#include "fractal/socketable_runtime.h"

#include <stdio.h>
#include <string.h>

static const fractal_module_descriptor mandelbrot_descriptor={
 1u,1u,"formula.mandelbrot.quadratic","Mandelbrot quadratic",
 FRACTAL_MODULE_FORMULA,FRACTAL_CAP_POINT_SCALAR,true
};
static const fractal_module_descriptor julia_descriptor={
 1u,1u,"formula.julia.quadratic","Julia quadratic",
 FRACTAL_MODULE_FORMULA,FRACTAL_CAP_POINT_SCALAR,true
};
static const fractal_module_descriptor scalar_compute_descriptor={
 1u,1u,FRACTAL_COMPUTATION_SCALAR_V1_ID,"Scalar computation v1",
 FRACTAL_MODULE_COMPUTE,FRACTAL_CAP_POINT_SCALAR|FRACTAL_CAP_ITERATION_FIELD|
 FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|
 FRACTAL_CAP_COMPUTE_CANCELLATION,true
};
static const fractal_module_descriptor compatibility_compute_descriptor={
 1u,1u,FRACTAL_COMPUTE_CONVENTIONAL_COMPATIBILITY_ID,
 "Conventional scalar C compatibility",FRACTAL_MODULE_COMPUTE,
 FRACTAL_CAP_POINT_SCALAR|FRACTAL_CAP_ITERATION_FIELD|
 FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|
 FRACTAL_CAP_COMPUTE_CANCELLATION,true
};

static fractal_result validate_parameter_shape(const fractal_formula_parameters *parameters,
 const char *type_id,size_t size){
 if(!parameters||!parameters->type_id||strcmp(parameters->type_id,type_id)||
    !parameters->data||parameters->size!=size)return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static fractal_result positive_binary64(const fractal_numeric_vtable *numeric,
 double value){
 fractal_scalar converted,zero;
 fractal_numeric_order_v1 order;
 if(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)!=
     FRACTAL_OK||numeric->from_binary64(value,&converted)!=FRACTAL_OK||
    numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    numeric->real_compare(&converted,&zero,&order)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return order==FRACTAL_NUMERIC_ORDER_GREATER?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

static fractal_result validate_mandelbrot(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters){
 const fractal_mandelbrot_parameters *value;
 if(validate_parameter_shape(parameters,mandelbrot_descriptor.module_id,sizeof(*value))!=
    FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 return positive_binary64(numeric,value->escape_radius);
}

static fractal_result validate_julia(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters){
 const fractal_julia_parameters *value;
 fractal_scalar converted;
 if(validate_parameter_shape(parameters,julia_descriptor.module_id,sizeof(*value))!=
    FRACTAL_OK)return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)!=
     FRACTAL_OK||numeric->from_binary64(value->constant_real,&converted)!=FRACTAL_OK||
    numeric->from_binary64(value->constant_imaginary,&converted)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return positive_binary64(numeric,value->escape_radius);
}

static size_t formula_state_size(void){return sizeof(fractal_complex_state);}
static size_t formula_state_alignment(void){return _Alignof(fractal_complex_state);}

static fractal_result initialize_radius(const fractal_numeric_vtable *numeric,
 double radius,fractal_scalar *radius_squared){
 fractal_scalar converted;
 if(numeric->from_binary64(radius,&converted)!=FRACTAL_OK||
    numeric->real_multiply(&converted,&converted,radius_squared)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static fractal_result initialize_mandelbrot(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,const fractal_numeric_complex *coordinate,
 void *output){
 const fractal_mandelbrot_parameters *value;
 fractal_complex_state *state=output;
 fractal_scalar zero;
 if(!coordinate||!state||validate_mandelbrot(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    numeric->complex_construct(&zero,&zero,&state->z)!=FRACTAL_OK||
    initialize_radius(numeric,value->escape_radius,&state->radius_squared)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state->c=*coordinate;
 return FRACTAL_OK;
}

static fractal_result initialize_julia(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,const fractal_numeric_complex *coordinate,
 void *output){
 const fractal_julia_parameters *value;
 fractal_complex_state *state=output;
 fractal_scalar real,imaginary;
 if(!coordinate||!state||validate_julia(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 if(numeric->from_binary64(value->constant_real,&real)!=FRACTAL_OK||
    numeric->from_binary64(value->constant_imaginary,&imaginary)!=FRACTAL_OK||
    numeric->complex_construct(&real,&imaginary,&state->c)!=FRACTAL_OK||
    initialize_radius(numeric,value->escape_radius,&state->radius_squared)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state->z=*coordinate;
 return FRACTAL_OK;
}

static fractal_result formula_step(const fractal_numeric_vtable *numeric,void *value){
 fractal_complex_state *state=value;
 fractal_numeric_complex squared,next;
 if(!numeric||!state)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(numeric->complex_square(&state->z,&squared)!=FRACTAL_OK||
    numeric->complex_add(&squared,&state->c,&next)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 state->z=next;
 return FRACTAL_OK;
}

static fractal_result formula_classify(const fractal_numeric_vtable *numeric,
 const void *value,fractal_point_class *classification){
 const fractal_complex_state *state=value;
 fractal_scalar magnitude_squared;
 bool escaped;
 if(!numeric||!state||!classification)return FRACTAL_ERROR_INVALID_ARGUMENT;
 if(numeric->squared_magnitude(&state->z,&magnitude_squared)!=FRACTAL_OK||
    numeric->bailout_exceeded(&magnitude_squared,&state->radius_squared,&escaped)!=FRACTAL_OK){
  *classification=FRACTAL_CLASS_FAILED;
  return FRACTAL_OK;
 }
 *classification=escaped?FRACTAL_CLASS_ESCAPED:FRACTAL_CLASS_UNRESOLVED;
 return FRACTAL_OK;
}

static fractal_result serialize_mandelbrot(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,char *output,size_t capacity,
 size_t *length){
 const fractal_mandelbrot_parameters *value;
 int count;
 if(validate_mandelbrot(numeric,parameters)!=FRACTAL_OK||!length)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 count=snprintf(output,capacity,"{\"escape_radius\":%.17g}",value->escape_radius);
 *length=count<0?0u:(size_t)count;
 return count<0||!output||capacity<=(size_t)count?
  FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

static fractal_result serialize_julia(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,char *output,size_t capacity,
 size_t *length){
 const fractal_julia_parameters *value;
 int count;
 if(validate_julia(numeric,parameters)!=FRACTAL_OK||!length)
  return FRACTAL_ERROR_INVALID_SPEC;
 value=parameters->data;
 count=snprintf(output,capacity,
  "{\"constant_imaginary\":%.17g,\"constant_real\":%.17g,\"escape_radius\":%.17g}",
  value->constant_imaginary,value->constant_real,value->escape_radius);
 *length=count<0?0u:(size_t)count;
 return count<0||!output||capacity<=(size_t)count?
  FRACTAL_ERROR_BUFFER_TOO_SMALL:FRACTAL_OK;
}

const fractal_formula_vtable fractal_formula_mandelbrot={
 &mandelbrot_descriptor,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 validate_mandelbrot,formula_state_size,formula_state_alignment,initialize_mandelbrot,
 formula_step,formula_classify,serialize_mandelbrot
};
const fractal_formula_vtable fractal_formula_julia={
 &julia_descriptor,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,
 validate_julia,formula_state_size,formula_state_alignment,initialize_julia,
 formula_step,formula_classify,serialize_julia
};

static fractal_result point_numeric(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric,const fractal_formula_parameters *parameters,
 const fractal_numeric_complex *coordinate,uint32_t budget,
 const fractal_cancellation *cancellation,fractal_point_result_compact *output){
 fractal_complex_state state;
 uint32_t iteration;
 if(!formula||!numeric||!coordinate||!output||!budget||
    formula->state_size()>sizeof(state)||
    formula->validate_parameters(numeric,parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(output,0,sizeof(*output));
 output->classification=FRACTAL_CLASS_UNRESOLVED;
 if(formula->initialize_state(numeric,parameters,coordinate,&state)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 for(iteration=1;iteration<=budget;iteration++){
  fractal_point_class classification;
  if(fractal_cancellation_is_requested(cancellation)){
   output->classification=FRACTAL_CLASS_CANCELLED;
   return FRACTAL_ERROR_CANCELLED;
  }
  if(formula->step(numeric,&state)!=FRACTAL_OK){
   output->classification=FRACTAL_CLASS_FAILED;
   return FRACTAL_OK;
  }
  output->steps=iteration;
  if(formula->classify(numeric,&state,&classification)!=FRACTAL_OK)
   return FRACTAL_ERROR_INVALID_SPEC;
  if(classification==FRACTAL_CLASS_ESCAPED){
   output->classification=(uint8_t)classification;
   return FRACTAL_OK;
  }
 }
 return FRACTAL_OK;
}

static fractal_result point(const fractal_formula_vtable *formula,
 const fractal_numeric_vtable *numeric,const fractal_formula_parameters *parameters,
 double real,double imaginary,uint32_t budget,const fractal_cancellation *cancellation,
 fractal_point_result_compact *output){
 fractal_scalar converted_real,converted_imaginary;
 fractal_numeric_complex coordinate;
 if(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)!=
     FRACTAL_OK||numeric->from_binary64(real,&converted_real)!=FRACTAL_OK||
    numeric->from_binary64(imaginary,&converted_imaginary)!=FRACTAL_OK||
    numeric->complex_construct(&converted_real,&converted_imaginary,&coordinate)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_ARGUMENT;
 return point_numeric(formula,numeric,parameters,&coordinate,budget,cancellation,output);
}

static uint64_t computation_problem_identity(const fractal_compute_vtable *compute,
 const fractal_computation_problem_v1 *problem){
 char parameters[256],text[1280];
 size_t length=0;
 int count;
 uint64_t numeric_execution_identity;
 if(!compute||!problem||!problem->formula||!problem->numeric||
    !problem->formula->serialize_parameters||
    problem->formula->serialize_parameters(problem->numeric,&problem->parameters,
     parameters,sizeof(parameters),&length)!=FRACTAL_OK)return 0;
 numeric_execution_identity=fractal_numeric_execution_identity_v1(problem->numeric);
 if(!numeric_execution_identity)return 0;
 count=snprintf(text,sizeof(text),
  "compute-work-unit|abi=%u|contract=%u|compute=%s@%u|numeric=%s@%u|numeric-abi=%u|numeric-caps=%016llx|numeric-execution=%016llx|formula=%s@%u|parameters=%.*s|max=%u|center=%.17g,%.17g|scale=%.17g|field=%u,%u,%zu,%u,%u",
  problem->abi_version,problem->contract_version,compute->descriptor->module_id,
  compute->descriptor->module_version,problem->numeric->descriptor->module_id,
  problem->numeric->descriptor->module_version,problem->numeric->abi_version,
  (unsigned long long)problem->numeric->capability_flags,
  (unsigned long long)numeric_execution_identity,problem->formula->descriptor->module_id,
  problem->formula->descriptor->module_version,(int)length,parameters,
  problem->maximum_steps,problem->center_real,problem->center_imaginary,problem->scale,
  problem->field.width,problem->field.height,problem->field.stride,
  (unsigned)problem->field.format,problem->field.flags);
 return count<0||(size_t)count>=sizeof(text)?0:
  fractal_checksum64(text,(size_t)count);
}

static fractal_result validate_view(const fractal_numeric_vtable *numeric,
 const fractal_computation_problem_v1 *problem){
 fractal_scalar converted,zero;
 fractal_numeric_order_v1 order;
 if(numeric->from_binary64(problem->center_real,&converted)!=FRACTAL_OK||
    numeric->from_binary64(problem->center_imaginary,&converted)!=FRACTAL_OK||
    numeric->from_binary64(problem->scale,&converted)!=FRACTAL_OK||
    numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)!=FRACTAL_OK||
    numeric->real_compare(&converted,&zero,&order)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return order==FRACTAL_NUMERIC_ORDER_GREATER?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

fractal_result fractal_computation_problem_validate_v1(const fractal_compute_vtable *compute,
 const fractal_computation_problem_v1 *problem){
 size_t bytes;
 uint64_t identity,required_numeric;
 if(!compute||!problem||!compute->descriptor||!problem->formula||!problem->numeric||
    !problem->formula->descriptor||!problem->formula->validate_parameters||
    !problem->formula->state_size||!problem->formula->state_alignment||
    !problem->formula->initialize_state||!problem->formula->step||
    !problem->formula->classify||!problem->formula->serialize_parameters||
    problem->abi_version!=FRACTAL_COMPUTATION_ABI_VERSION||
    problem->contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    compute->contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    !compute->execute||!compute->point||
    fractal_module_descriptor_validate(compute->descriptor,FRACTAL_MODULE_COMPUTE)!=FRACTAL_OK||
    fractal_module_descriptor_validate(problem->formula->descriptor,FRACTAL_MODULE_FORMULA)!=FRACTAL_OK||
    !problem->maximum_steps||
    fractal_field_descriptor_validate(&problem->field,sizeof(fractal_point_result_compact),
     &bytes)!=FRACTAL_OK||
    problem->field.format!=FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1||
    (compute->descriptor->capability_flags&(FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|
     FRACTAL_CAP_COMPUTE_CALLER_FIELD|FRACTAL_CAP_COMPUTE_CANCELLATION))!=
     (FRACTAL_CAP_COMPUTE_CONTIGUOUS_ROWS|FRACTAL_CAP_COMPUTE_CALLER_FIELD|
      FRACTAL_CAP_COMPUTE_CANCELLATION)||
    (compute->descriptor->capability_flags&compute->required_field_capabilities)!=
     compute->required_field_capabilities||
    (problem->formula->descriptor->capability_flags&compute->required_formula_capabilities)!=
     compute->required_formula_capabilities)
  return FRACTAL_ERROR_INVALID_SPEC;
 required_numeric=compute->required_numeric_capabilities|
  problem->formula->required_numeric_capabilities;
 if(fractal_numeric_validate_v1(problem->numeric,required_numeric)!=FRACTAL_OK||
    validate_view(problem->numeric,problem)!=FRACTAL_OK||
    problem->formula->validate_parameters(problem->numeric,&problem->parameters)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 identity=computation_problem_identity(compute,problem);
 return identity&&identity==problem->identity?FRACTAL_OK:FRACTAL_ERROR_INVALID_SPEC;
}

fractal_result fractal_computation_problem_init_v1(const fractal_runtime_modules *runtime,
 const fractal_job_spec *job,const fractal_field_descriptor *descriptor,
 fractal_computation_problem_v1 *output){
 if(!runtime||!job||!descriptor||!output||!runtime->compute||!runtime->formula||
    !runtime->numeric)return FRACTAL_ERROR_INVALID_ARGUMENT;
 *output=(fractal_computation_problem_v1){
  FRACTAL_COMPUTATION_ABI_VERSION,FRACTAL_COMPUTATION_CONTRACT_VERSION,
  runtime->formula,runtime->numeric,job->problem.parameters,job->problem.maximum_steps,
  job->view.center_real,job->view.center_imaginary,job->view.scale,*descriptor,0
 };
 output->identity=computation_problem_identity(runtime->compute,output);
 return fractal_computation_problem_validate_v1(runtime->compute,output);
}

static fractal_result coordinate_for_sample(const fractal_numeric_vtable *numeric,
 const fractal_computation_problem_v1 *problem,uint32_t x,uint32_t y,
 fractal_numeric_complex *coordinate){
 fractal_scalar width,height,x_value,y_value,half,center_real,center_imaginary,scale;
 fractal_scalar aspect,real,imaginary,temporary;
 if(numeric->from_uint32(problem->field.width,&width)!=FRACTAL_OK||
    numeric->from_uint32(problem->field.height,&height)!=FRACTAL_OK||
    numeric->from_uint32(x,&x_value)!=FRACTAL_OK||
    numeric->from_uint32(y,&y_value)!=FRACTAL_OK||
    numeric->constant(FRACTAL_NUMERIC_CONSTANT_HALF,&half)!=FRACTAL_OK||
    numeric->from_binary64(problem->center_real,&center_real)!=FRACTAL_OK||
    numeric->from_binary64(problem->center_imaginary,&center_imaginary)!=FRACTAL_OK||
    numeric->from_binary64(problem->scale,&scale)!=FRACTAL_OK||
    numeric->real_divide(&width,&height,&aspect)!=FRACTAL_OK||
    numeric->real_add(&x_value,&half,&temporary)!=FRACTAL_OK||
    numeric->real_divide(&temporary,&width,&temporary)!=FRACTAL_OK||
    numeric->real_subtract(&temporary,&half,&temporary)!=FRACTAL_OK||
    numeric->real_multiply(&temporary,&scale,&temporary)!=FRACTAL_OK||
    numeric->real_multiply(&temporary,&aspect,&temporary)!=FRACTAL_OK||
    numeric->real_add(&center_real,&temporary,&real)!=FRACTAL_OK||
    numeric->real_add(&y_value,&half,&temporary)!=FRACTAL_OK||
    numeric->real_divide(&temporary,&height,&temporary)!=FRACTAL_OK||
    numeric->real_subtract(&half,&temporary,&temporary)!=FRACTAL_OK||
    numeric->real_multiply(&temporary,&scale,&temporary)!=FRACTAL_OK||
    numeric->real_add(&center_imaginary,&temporary,&imaginary)!=FRACTAL_OK||
    numeric->complex_construct(&real,&imaginary,coordinate)!=FRACTAL_OK)
  return FRACTAL_ERROR_INVALID_SPEC;
 return FRACTAL_OK;
}

static fractal_result computation_execute(const fractal_compute_vtable *compute,
 const fractal_computation_request_v1 *request,fractal_computation_result_v1 *output){
 const fractal_computation_problem_v1 *problem;
 const fractal_sealed_work_unit_v1 *assignment;
 uint32_t x,y;
 if(!output)return FRACTAL_ERROR_INVALID_ARGUMENT;
 memset(output,0,sizeof(*output));
 output->status=FRACTAL_COMPUTATION_NOT_STARTED;
 output->result=FRACTAL_ERROR_INVALID_ARGUMENT;
 if(!request||request->abi_version!=FRACTAL_COMPUTATION_ABI_VERSION||
    request->contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    !request->problem||!request->assignment)return FRACTAL_ERROR_INVALID_ARGUMENT;
 problem=request->problem;assignment=request->assignment;
 output->assignment_identity=assignment->identity;
 output->sequence=assignment->sequence;
 if(fractal_computation_problem_validate_v1(compute,problem)!=FRACTAL_OK||
    fractal_mutable_field_view_validate(&request->destination,
     sizeof(fractal_point_result_compact))!=FRACTAL_OK||
    memcmp(&request->destination.descriptor,&problem->field,sizeof(problem->field))||
    assignment->abi_version!=FRACTAL_COMPUTATION_ABI_VERSION||
    assignment->contract_version!=FRACTAL_COMPUTATION_CONTRACT_VERSION||
    assignment->work_unit_identity!=problem->identity||
    assignment->computation_identity!=fractal_module_identity_v1(compute->descriptor)||
    assignment->numeric_identity!=fractal_module_identity_v1(problem->numeric->descriptor)||
    assignment->numeric_abi_version!=problem->numeric->abi_version||
    assignment->numeric_capability_flags!=problem->numeric->capability_flags||
    assignment->formula_identity!=fractal_module_identity_v1(problem->formula->descriptor)||
    assignment->computation_version!=compute->descriptor->module_version||
    assignment->numeric_version!=problem->numeric->descriptor->module_version||
    assignment->formula_version!=problem->formula->descriptor->module_version||
    assignment->field_format!=(uint32_t)problem->field.format||
    (assignment->cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION&&
     assignment->cancellation_mode!=FRACTAL_COMPUTATION_CANCEL_ROW_BOUNDARY)||
    assignment->width!=problem->field.width||assignment->height!=problem->field.height||
    !assignment->worker_count||assignment->sequence>=assignment->worker_count||
    assignment->row_begin>assignment->row_end||assignment->row_end>assignment->height||
    assignment->sample_begin!=(uint64_t)assignment->row_begin*assignment->width||
    assignment->sample_end!=(uint64_t)assignment->row_end*assignment->width||
    assignment->identity!=fractal_scheduler_work_unit_identity_v1(assignment)){
  output->status=FRACTAL_COMPUTATION_FAILED;
  output->result=FRACTAL_ERROR_INVALID_SPEC;
  return output->result;
 }
 if(fractal_cancellation_is_requested(request->cancellation)){
  output->status=FRACTAL_COMPUTATION_CANCELLED;
  output->result=FRACTAL_ERROR_CANCELLED;
  return output->result;
 }
 for(y=assignment->row_begin;y<assignment->row_end;y++){
  fractal_point_result_compact *row;
  if(fractal_cancellation_is_requested(request->cancellation)){
   output->status=FRACTAL_COMPUTATION_CANCELLED;
   output->result=FRACTAL_ERROR_CANCELLED;
   return output->result;
  }
  row=(fractal_point_result_compact*)((unsigned char*)request->destination.data+
   (size_t)y*problem->field.stride);
  for(x=0;x<problem->field.width;x++){
   fractal_numeric_complex coordinate;
   const fractal_cancellation *point_cancellation=
    assignment->cancellation_mode==FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION?
     request->cancellation:NULL;
   fractal_result result;
   if(coordinate_for_sample(problem->numeric,problem,x,y,&coordinate)!=FRACTAL_OK)
    result=FRACTAL_ERROR_INVALID_SPEC;
   else result=point_numeric(problem->formula,problem->numeric,&problem->parameters,
     &coordinate,problem->maximum_steps,point_cancellation,&row[x]);
   if(result!=FRACTAL_OK){
    output->status=result==FRACTAL_ERROR_CANCELLED?
     FRACTAL_COMPUTATION_CANCELLED:FRACTAL_COMPUTATION_FAILED;
    output->result=result;
    return result;
   }
   output->samples_completed++;
  }
  output->rows_completed++;
 }
 output->status=FRACTAL_COMPUTATION_SUCCEEDED;
 output->result=FRACTAL_OK;
 return FRACTAL_OK;
}

static fractal_result scalar_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *output);
static fractal_result compatibility_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *output);

const fractal_compute_vtable fractal_compute_scalar_v1={
 &scalar_compute_descriptor,FRACTAL_CAP_POINT_SCALAR,
 FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,FRACTAL_CAP_ITERATION_FIELD,
 FRACTAL_COMPUTATION_CONTRACT_VERSION,point,scalar_execute
};
const fractal_compute_vtable fractal_compute_conventional={
 &compatibility_compute_descriptor,FRACTAL_CAP_POINT_SCALAR,
 FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES,FRACTAL_CAP_ITERATION_FIELD,
 FRACTAL_COMPUTATION_CONTRACT_VERSION,point,compatibility_execute
};

static fractal_result scalar_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *output){
 return computation_execute(&fractal_compute_scalar_v1,request,output);
}

static fractal_result compatibility_execute(const fractal_computation_request_v1 *request,
 fractal_computation_result_v1 *output){
 return computation_execute(&fractal_compute_conventional,request,output);
}
