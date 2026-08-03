#include "fractal/installed_modules.h"

#include <stdio.h>
#include <string.h>

static int failures;
#define CHECK(value) do{if(!(value)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value);failures++;}}while(0)

static uint64_t scalar_bits(const fractal_scalar *value){
 uint64_t bits=0;
 memcpy(&bits,value->storage,sizeof(bits));
 return bits;
}

static double scalar_value(const fractal_scalar *value){
 double result=0.0;
 CHECK(fractal_numeric_binary64_v1.to_binary64(value,&result)==FRACTAL_OK);
 return result;
}

static void binary64_contract(void){
 const fractal_numeric_vtable *numeric=&fractal_numeric_binary64_v1;
 fractal_scalar zero,one,two,half,a,b,result,magnitude,radius_squared;
 fractal_numeric_complex left,right,added,subtracted,multiplied,squared;
 fractal_numeric_order_v1 order;
 bool equal=false,escaped=false;
 char serialized[32];
 size_t length=0;
 uint64_t identity;
 CHECK(!strcmp(numeric->descriptor->module_id,FRACTAL_NUMERIC_BINARY64_V1_ID));
 CHECK(numeric->abi_version==FRACTAL_NUMERIC_ABI_VERSION&&
  numeric->contract_version==FRACTAL_NUMERIC_CONTRACT_VERSION&&
  numeric->capability_flags==FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES&&
  numeric->precision_bits==53&&numeric->scalar_size==8&&numeric->scalar_alignment==8);
 CHECK(fractal_numeric_validate_v1(numeric,FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES)==
  FRACTAL_OK);
 identity=fractal_numeric_execution_identity_v1(numeric);
 CHECK(identity==UINT64_C(0x2f300bf0f7ea5244)&&
  identity==fractal_numeric_execution_identity_v1(numeric));
 CHECK(fractal_module_identity_v1(numeric->descriptor)==UINT64_C(0xf554b96377ce0dbb));

 CHECK(numeric->constant(FRACTAL_NUMERIC_CONSTANT_ZERO,&zero)==FRACTAL_OK&&
  scalar_bits(&zero)==UINT64_C(0x0000000000000000));
 CHECK(numeric->constant(FRACTAL_NUMERIC_CONSTANT_ONE,&one)==FRACTAL_OK&&
  scalar_bits(&one)==UINT64_C(0x3ff0000000000000));
 CHECK(numeric->constant(FRACTAL_NUMERIC_CONSTANT_TWO,&two)==FRACTAL_OK&&
  scalar_bits(&two)==UINT64_C(0x4000000000000000));
 CHECK(numeric->constant(FRACTAL_NUMERIC_CONSTANT_HALF,&half)==FRACTAL_OK&&
  scalar_bits(&half)==UINT64_C(0x3fe0000000000000));
 CHECK(numeric->from_binary64(1.5,&a)==FRACTAL_OK&&
  scalar_bits(&a)==UINT64_C(0x3ff8000000000000));
 CHECK(numeric->from_uint32(2,&b)==FRACTAL_OK&&scalar_bits(&b)==scalar_bits(&two));
 CHECK(numeric->real_add(&a,&b,&result)==FRACTAL_OK&&scalar_value(&result)==3.5);
 CHECK(numeric->real_subtract(&a,&b,&result)==FRACTAL_OK&&scalar_value(&result)==-0.5);
 CHECK(numeric->real_multiply(&a,&b,&result)==FRACTAL_OK&&scalar_value(&result)==3.0);
 CHECK(numeric->real_divide(&a,&b,&result)==FRACTAL_OK&&scalar_value(&result)==0.75);
 CHECK(numeric->real_compare(&a,&b,&order)==FRACTAL_OK&&order==FRACTAL_NUMERIC_ORDER_LESS);
 CHECK(numeric->real_equal(&two,&b,&equal)==FRACTAL_OK&&equal&&
  numeric->real_is_finite(&a));
 CHECK(numeric->serialize_real(&a,serialized,sizeof(serialized),&length)==FRACTAL_OK&&
  length==16&&!memcmp(serialized,"3ff8000000000000",16));

 CHECK(numeric->from_binary64(-2.0,&b)==FRACTAL_OK&&
  numeric->complex_construct(&a,&b,&left)==FRACTAL_OK);
 CHECK(numeric->complex_construct(&two,&half,&right)==FRACTAL_OK);
 CHECK(numeric->complex_add(&left,&right,&added)==FRACTAL_OK&&
  scalar_value(&added.real)==3.5&&scalar_value(&added.imaginary)==-1.5);
 CHECK(numeric->complex_subtract(&left,&right,&subtracted)==FRACTAL_OK&&
  scalar_value(&subtracted.real)==-0.5&&scalar_value(&subtracted.imaginary)==-2.5);
 CHECK(numeric->complex_multiply(&left,&right,&multiplied)==FRACTAL_OK&&
  scalar_value(&multiplied.real)==4.0&&scalar_value(&multiplied.imaginary)==-3.25);
 CHECK(numeric->complex_square(&left,&squared)==FRACTAL_OK&&
  scalar_value(&squared.real)==-1.75&&scalar_value(&squared.imaginary)==-6.0);
 CHECK(numeric->squared_magnitude(&left,&magnitude)==FRACTAL_OK&&
  scalar_value(&magnitude)==6.25);
 CHECK(numeric->real_multiply(&two,&two,&radius_squared)==FRACTAL_OK&&
  numeric->bailout_exceeded(&magnitude,&radius_squared,&escaped)==FRACTAL_OK&&escaped);
 printf("NUMERIC execution=%016llx module=%016llx\n",(unsigned long long)identity,
  (unsigned long long)fractal_module_identity_v1(numeric->descriptor));
}

static fractal_runtime_selection runtime_selection(const char *numeric_id){
 fractal_runtime_selection selected={0};
 selected.formula=FRACTAL_FORMULA_MANDELBROT_V1_ID;
 selected.numeric=numeric_id;
 selected.compute=FRACTAL_COMPUTATION_SCALAR_V1_ID;
 selected.refinement="refinement.none";
 selected.scheduler=FRACTAL_SCHEDULER_SERIAL_V1_ID;
 selected.raster="raster.native.iteration-bgr8";
 selected.encoder="encoder.bmp.v3";
 selected.memory="memory.system.scoped";
 selected.telemetry="telemetry.noop";
 selected.platform="platform.host.c11";
 selected.analysis_mode=FRACTAL_ANALYSIS_BORROW_INPUT;
 selected.scheduler_worker_count=1;
 return selected;
}

static void registry_and_compatibility(void){
 fractal_module_registry installed,extended;
 fractal_module_registration entries[FRACTAL_REGISTRY_MAX_MODULES];
 fractal_module_descriptor unavailable_descriptor=*fractal_numeric_binary64_v1.descriptor;
 fractal_numeric_vtable unavailable=fractal_numeric_binary64_v1;
 fractal_numeric_vtable bad_numeric=fractal_numeric_binary64_v1;
 fractal_compute_vtable bad_compute=fractal_compute_scalar_v1;
 fractal_memory_backend memory;
 fractal_runtime_modules runtime;
 fractal_runtime_selection selected=runtime_selection(FRACTAL_NUMERIC_BINARY64_V1_ID);
 size_t i;
 CHECK(fractal_installed_modules_registry(&installed)==FRACTAL_OK);
 CHECK(installed.identity==UINT64_C(0x616b9a3b92e787e5)&&installed.count==22);
 CHECK(fractal_module_registry_count(&installed,FRACTAL_MODULE_NUMERIC)==1);
 CHECK(fractal_module_registry_find(&installed,FRACTAL_MODULE_NUMERIC,
  FRACTAL_NUMERIC_BINARY64_V1_ID)==fractal_numeric_binary64_v1.descriptor);
 CHECK(fractal_module_registry_implementation(&installed,FRACTAL_MODULE_NUMERIC,
  FRACTAL_NUMERIC_BINARY64_V1_ID)==&fractal_numeric_binary64_v1);
 CHECK(fractal_module_registry_find(&installed,FRACTAL_MODULE_NUMERIC,"numeric.binary64")==NULL);
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_OK&&
  runtime.numeric==&fractal_numeric_binary64_v1);
 selected.numeric="fractal.numeric.unknown.v1";
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==
  FRACTAL_ERROR_NOT_IMPLEMENTED);

 for(i=0;i<installed.count;i++)entries[i]=installed.entries[i];
 unavailable_descriptor.module_id="fractal.numeric.binary64.unavailable.v1";
 unavailable_descriptor.display_name="Unavailable Binary64 test numeric";
 unavailable_descriptor.available=false;
 unavailable.descriptor=&unavailable_descriptor;
 entries[installed.count]=(fractal_module_registration){&unavailable_descriptor,&unavailable};
 CHECK(fractal_module_registry_init(&extended,entries,installed.count+1u)==FRACTAL_OK);
 selected.numeric=unavailable_descriptor.module_id;
 CHECK(fractal_runtime_assemble(&extended,&selected,&memory,&runtime)==
  FRACTAL_ERROR_NOT_IMPLEMENTED);

 selected=runtime_selection(FRACTAL_NUMERIC_BINARY64_V1_ID);
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_OK);
 bad_numeric.abi_version=FRACTAL_NUMERIC_ABI_VERSION+1u;
 runtime.numeric=&bad_numeric;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 bad_numeric=fractal_numeric_binary64_v1;
 bad_numeric.capability_flags&=~FRACTAL_NUMERIC_CAP_COMPLEX_SQUARE;
 runtime.numeric=&bad_numeric;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 runtime.numeric=&fractal_numeric_binary64_v1;
 bad_compute.required_numeric_capabilities|=UINT64_C(1)<<63;
 runtime.compute=&bad_compute;
 CHECK(fractal_runtime_validate(&runtime)==FRACTAL_ERROR_INVALID_SPEC);
 fractal_memory_backend_shutdown(&memory);
 printf("NUMERIC registry=%016llx modules=%zu\n",
  (unsigned long long)installed.identity,installed.count);
}

static void work_unit_numeric_seal(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,&mandelbrot,
  sizeof(mandelbrot)};
 fractal_job_spec job={{FRACTAL_FORMULA_MANDELBROT_V1_ID,parameters,64},
  {-0.5,0.0,3.0,4,4},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","numeric-socket.bmp"}};
 fractal_runtime_modules runtime={0};
 fractal_field_descriptor field={4,4,4*sizeof(fractal_point_result_compact),
  FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
 fractal_computation_problem_v1 problem;
 fractal_sealed_work_unit_v1 first[2],repeat[2],changed[2];
 size_t count=0,repeat_count=0;
 runtime.formula=&fractal_formula_mandelbrot;
 runtime.numeric=&fractal_numeric_binary64_v1;
 runtime.compute=&fractal_compute_scalar_v1;
 CHECK(fractal_computation_problem_init_v1(&runtime,&job,&field,&problem)==FRACTAL_OK);
 CHECK(fractal_scheduler_decompose_computation_v1(runtime.compute,&problem,2,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,first,2,&count)==FRACTAL_OK);
 CHECK(fractal_scheduler_decompose_computation_v1(runtime.compute,&problem,2,
  FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,repeat,2,&repeat_count)==FRACTAL_OK&&
  count==repeat_count&&!memcmp(first,repeat,sizeof(first)));
 CHECK(problem.identity==UINT64_C(0xf56016b2b40fda1a));
 CHECK(first[0].identity==UINT64_C(0x0be251ff71555e6f)&&
  first[1].identity==UINT64_C(0x37b5e8b924fd930f)&&
  fractal_scheduler_work_unit_set_identity_v1(first,count)==UINT64_C(0xd02f6f67a4dd7f05));
 CHECK(first[0].numeric_identity==fractal_module_identity_v1(runtime.numeric->descriptor)&&
  first[0].numeric_abi_version==FRACTAL_NUMERIC_ABI_VERSION&&
  first[0].numeric_capability_flags==FRACTAL_NUMERIC_SCALAR_V1_REQUIRED_CAPABILITIES);
 memcpy(changed,first,sizeof(changed));changed[0].numeric_abi_version++;
 changed[0].identity=fractal_scheduler_work_unit_identity_v1(&changed[0]);
 CHECK(changed[0].identity!=first[0].identity&&
  fractal_scheduler_validate_computation_v1(runtime.compute,&problem,changed,2,2)==
   FRACTAL_ERROR_INVALID_SPEC);
 memcpy(changed,first,sizeof(changed));
 changed[0].numeric_capability_flags^=FRACTAL_NUMERIC_CAP_REAL_DIVIDE;
 changed[0].identity=fractal_scheduler_work_unit_identity_v1(&changed[0]);
 CHECK(changed[0].identity!=first[0].identity&&
  fractal_scheduler_validate_computation_v1(runtime.compute,&problem,changed,2,2)==
   FRACTAL_ERROR_INVALID_SPEC);
 printf("NUMERIC work=%016llx assignments=%016llx,%016llx set=%016llx\n",
  (unsigned long long)problem.identity,(unsigned long long)first[0].identity,
  (unsigned long long)first[1].identity,
  (unsigned long long)fractal_scheduler_work_unit_set_identity_v1(first,count));
}

static void frozen_module_identities(void){
 CHECK(fractal_module_identity_v1(fractal_formula_mandelbrot.descriptor)==
  UINT64_C(0x86d0d6293d4696cd));
 CHECK(fractal_module_identity_v1(fractal_formula_julia.descriptor)==
  UINT64_C(0x9b51aed5f291fcd6));
 CHECK(fractal_module_identity_v1(fractal_compute_scalar_v1.descriptor)==
  UINT64_C(0x0f4e80b9ea57acc9));
 CHECK(fractal_module_identity_v1(fractal_scheduler_serial_v1.descriptor)==
  UINT64_C(0x063fcb1076049dd8));
 CHECK(fractal_module_identity_v1(fractal_scheduler_thread_pool_v1.descriptor)==
  UINT64_C(0xd55b1ea157b8ae6a));
 CHECK(fractal_module_identity_v1(fractal_scheduler_serial.descriptor)==
  UINT64_C(0xeed3275b21239e7c));
 CHECK(fractal_module_identity_v1(fractal_compute_conventional.descriptor)==
  UINT64_C(0xb3a66e17abf7dc25));
 CHECK(fractal_module_identity_v1(fractal_analyzer_passthrough.descriptor)==
  UINT64_C(0x0cd0faf0b821b20f));
 CHECK(fractal_module_identity_v1(fractal_analyzer_escape_classification_summary.descriptor)==
  UINT64_C(0xbfe7cc4bda9e6987));
 CHECK(fractal_module_identity_v1(fractal_analyzer_iteration_histogram.descriptor)==
  UINT64_C(0x09564136615eac14));
 CHECK(fractal_module_identity_v1(fractal_analyzer_spatial_workload_grid.descriptor)==
  UINT64_C(0x9ba05e55019dd0d9));
 printf("MODULE formula-mandelbrot=%016llx formula-julia=%016llx compute=%016llx serial=%016llx thread-pool=%016llx\n",
  (unsigned long long)fractal_module_identity_v1(fractal_formula_mandelbrot.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_formula_julia.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_compute_scalar_v1.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_scheduler_serial_v1.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_scheduler_thread_pool_v1.descriptor));
 printf("MODULE serial-compat=%016llx compute-compat=%016llx analyzers=%016llx,%016llx,%016llx,%016llx\n",
  (unsigned long long)fractal_module_identity_v1(fractal_scheduler_serial.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_compute_conventional.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_analyzer_passthrough.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_analyzer_escape_classification_summary.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_analyzer_iteration_histogram.descriptor),
  (unsigned long long)fractal_module_identity_v1(fractal_analyzer_spatial_workload_grid.descriptor));
}

int main(void){
 binary64_contract();
 registry_and_compatibility();
 work_unit_numeric_seal();
 frozen_module_identities();
 return failures?1:0;
}
