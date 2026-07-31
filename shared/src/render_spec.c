#include "fractal/render_spec.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct json_writer { char *data; size_t capacity; size_t length; bool overflow; } json_writer;

static bool bounded_text(const char *text, size_t capacity) {
    return text != NULL && memchr(text, '\0', capacity) != NULL && text[0] != '\0';
}

static bool identifier_valid(const char *text, size_t capacity) {
    const unsigned char *cursor = (const unsigned char *)text;
    if (!bounded_text(text, capacity)) return false;
    while (*cursor != '\0') {
        if (!((*cursor >= 'a' && *cursor <= 'z') ||
              (*cursor >= 'A' && *cursor <= 'Z') ||
              (*cursor >= '0' && *cursor <= '9') || *cursor == '-' ||
              *cursor == '_' || *cursor == '.')) return false;
        ++cursor;
    }
    return true;
}

/* Canonical decimal grammar: -?(0|[1-9][0-9]*)(\.[0-9]*[1-9])?.
 * Requiring canonical input makes numeric equivalence unambiguous without silently
 * changing precision or depending on the process locale. */
static bool decimal_valid(const char *text, bool positive) {
    const unsigned char *p = (const unsigned char *)text;
    const unsigned char *fraction;
    if (!bounded_text(text, FRACTAL_DECIMAL_CAPACITY)) return false;
    if (*p == '-') {
        if (positive) return false;
        ++p;
    }
    if (*p == '0') {
        ++p;
        if (*p >= '0' && *p <= '9') return false;
    } else {
        if (*p < '1' || *p > '9') return false;
        while (*p >= '0' && *p <= '9') ++p;
    }
    if (*p == '.') {
        fraction = ++p;
        while (*p >= '0' && *p <= '9') ++p;
        if (p == fraction || p[-1] == '0') return false;
    }
    if (*p != '\0') return false;
    if (!positive && strcmp(text, "-0") == 0) return false;
    return !positive || strcmp(text, "0") != 0;
}

static void put_bytes(json_writer *writer, const char *data, size_t length) {
    if (length > SIZE_MAX - writer->length) { writer->overflow = true; return; }
    if (writer->data != NULL && writer->length < writer->capacity) {
        size_t available = writer->capacity - writer->length;
        size_t copy = length < available ? length : available;
        memcpy(writer->data + writer->length, data, copy);
    }
    writer->length += length;
    if (writer->length >= writer->capacity) writer->overflow = true;
}
static void put_text(json_writer *writer, const char *text) { put_bytes(writer, text, strlen(text)); }
static void put_uint(json_writer *writer, uint32_t value) {
    char number[16]; int count = snprintf(number, sizeof(number), "%u", (unsigned)value);
    if (count > 0) put_bytes(writer, number, (size_t)count);
}
static void put_json_string(json_writer *writer, const char *text) {
    static const char hex[] = "0123456789abcdef";
    put_text(writer, "\"");
    for (; *text != '\0'; ++text) {
        unsigned char c = (unsigned char)*text;
        if (c == '"' || c == '\\') { char escaped[2] = {'\\', (char)c}; put_bytes(writer, escaped, 2u); }
        else if (c < 0x20u) { char escaped[6] = {'\\','u','0','0',hex[c >> 4u],hex[c & 15u]}; put_bytes(writer, escaped, 6u); }
        else put_bytes(writer, (const char *)&c, 1u);
    }
    put_text(writer, "\"");
}

const char *fractal_numerical_precision_string(fractal_numerical_precision precision) {
    switch (precision) {
    case FRACTAL_PRECISION_FLOAT32: return "float32";
    case FRACTAL_PRECISION_FLOAT64: return "float64";
    case FRACTAL_PRECISION_LONG_DOUBLE: return "long-double";
    case FRACTAL_PRECISION_ARBITRARY_DECIMAL: return "arbitrary-decimal";
    default: return NULL;
    }
}

fractal_result fractal_render_spec_init_default(fractal_render_spec *spec) {
    if (spec == NULL) return FRACTAL_ERROR_INVALID_ARGUMENT;
    memset(spec, 0, sizeof(*spec));
    strcpy(spec->fractal_type, "mandelbrot"); strcpy(spec->center_real, "-0.75");
    strcpy(spec->center_imaginary, "0"); strcpy(spec->scale, "4");
    spec->image_width = 800u; spec->image_height = 600u; spec->maximum_iterations = 150u;
    strcpy(spec->bailout_radius, "2"); spec->numerical_precision = FRACTAL_PRECISION_FLOAT64;
    spec->tile_width = 128u; spec->tile_height = 128u; spec->supersampling = 1u;
    strcpy(spec->palette_identifier, "dstep");
    return FRACTAL_OK;
}

fractal_result fractal_render_spec_validate(const fractal_render_spec *spec) {
    if (spec == NULL) return FRACTAL_ERROR_INVALID_ARGUMENT;
    if (!identifier_valid(spec->fractal_type, sizeof(spec->fractal_type)) ||
        !identifier_valid(spec->palette_identifier, sizeof(spec->palette_identifier)) ||
        !decimal_valid(spec->center_real, false) || !decimal_valid(spec->center_imaginary, false) ||
        !decimal_valid(spec->scale, true) || !decimal_valid(spec->bailout_radius, true) ||
        spec->image_width == 0u || spec->image_height == 0u || spec->maximum_iterations == 0u ||
        spec->tile_width == 0u || spec->tile_height == 0u || spec->tile_width > spec->image_width ||
        spec->tile_height > spec->image_height || spec->supersampling == 0u ||
        fractal_numerical_precision_string(spec->numerical_precision) == NULL)
        return FRACTAL_ERROR_INVALID_SPEC;
    if (spec->animation.enabled &&
        (spec->animation.frame_count == 0u || !decimal_valid(spec->animation.frames_per_second, true) ||
         !decimal_valid(spec->animation.zoom_multiplier_per_frame, true)))
        return FRACTAL_ERROR_INVALID_SPEC;
    return FRACTAL_OK;
}

fractal_result fractal_render_spec_serialize_canonical_json(const fractal_render_spec *spec,
 char *output, size_t capacity, size_t *output_length) {
    json_writer w = {output, capacity, 0u, false};
    fractal_result result;
    if (output_length == NULL || (output == NULL && capacity != 0u)) return FRACTAL_ERROR_INVALID_ARGUMENT;
    result = fractal_render_spec_validate(spec); if (result != FRACTAL_OK) return result;
#define KEY(name) put_text(&w, "\"" name "\":")
    put_text(&w, "{"); KEY("animation");
    if (!spec->animation.enabled) put_text(&w, "null"); else {
        put_text(&w, "{"); KEY("frame_count"); put_uint(&w, spec->animation.frame_count);
        put_text(&w, ","); KEY("frames_per_second"); put_json_string(&w, spec->animation.frames_per_second);
        put_text(&w, ","); KEY("zoom_multiplier_per_frame"); put_json_string(&w, spec->animation.zoom_multiplier_per_frame); put_text(&w, "}");
    }
    put_text(&w, ","); KEY("bailout_radius"); put_json_string(&w, spec->bailout_radius);
    put_text(&w, ","); KEY("center_imaginary"); put_json_string(&w, spec->center_imaginary);
    put_text(&w, ","); KEY("center_real"); put_json_string(&w, spec->center_real);
    put_text(&w, ","); KEY("fractal_type"); put_json_string(&w, spec->fractal_type);
    put_text(&w, ","); KEY("image_height"); put_uint(&w, spec->image_height);
    put_text(&w, ","); KEY("image_width"); put_uint(&w, spec->image_width);
    put_text(&w, ","); KEY("maximum_iterations"); put_uint(&w, spec->maximum_iterations);
    put_text(&w, ","); KEY("numerical_precision"); put_json_string(&w, fractal_numerical_precision_string(spec->numerical_precision));
    put_text(&w, ","); KEY("palette_identifier"); put_json_string(&w, spec->palette_identifier);
    put_text(&w, ","); KEY("scale"); put_json_string(&w, spec->scale);
    put_text(&w, ","); KEY("supersampling"); put_uint(&w, spec->supersampling);
    put_text(&w, ","); KEY("tile_height"); put_uint(&w, spec->tile_height);
    put_text(&w, ","); KEY("tile_width"); put_uint(&w, spec->tile_width); put_text(&w, "}");
#undef KEY
    *output_length = w.length;
    if (output != NULL && capacity > w.length) output[w.length] = '\0';
    if (w.overflow || capacity <= w.length) return FRACTAL_ERROR_BUFFER_TOO_SMALL;
    return FRACTAL_OK;
}

bool fractal_render_spec_equal(const fractal_render_spec *left, const fractal_render_spec *right) {
    if (left == NULL || right == NULL) return false;
    if (fractal_render_spec_validate(left) != FRACTAL_OK ||
        fractal_render_spec_validate(right) != FRACTAL_OK) return false;
    return strcmp(left->fractal_type, right->fractal_type) == 0 &&
        strcmp(left->center_real, right->center_real) == 0 &&
        strcmp(left->center_imaginary, right->center_imaginary) == 0 &&
        strcmp(left->scale, right->scale) == 0 &&
        left->image_width == right->image_width && left->image_height == right->image_height &&
        left->maximum_iterations == right->maximum_iterations &&
        strcmp(left->bailout_radius, right->bailout_radius) == 0 &&
        left->numerical_precision == right->numerical_precision &&
        left->tile_width == right->tile_width && left->tile_height == right->tile_height &&
        left->supersampling == right->supersampling &&
        strcmp(left->palette_identifier, right->palette_identifier) == 0 &&
        left->animation.enabled == right->animation.enabled &&
        (!left->animation.enabled ||
         (left->animation.frame_count == right->animation.frame_count &&
          strcmp(left->animation.frames_per_second, right->animation.frames_per_second) == 0 &&
          strcmp(left->animation.zoom_multiplier_per_frame,
                 right->animation.zoom_multiplier_per_frame) == 0));
}
