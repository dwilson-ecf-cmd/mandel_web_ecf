#include "fractal/installed_modules.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures;
#define CHECK(value) do{if(!(value)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value);failures++;}}while(0)

static fractal_runtime_selection selection(const char *formula_id,const char *numeric_id){
 fractal_runtime_selection selected={0};
 selected.formula=formula_id;
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

static fractal_numeric_complex complex_value(double real,double imaginary){
 fractal_numeric_complex result;
 fractal_scalar r,i;
 CHECK(fractal_numeric_binary64_v1.from_binary64(real,&r)==FRACTAL_OK);
 CHECK(fractal_numeric_binary64_v1.from_binary64(imaginary,&i)==FRACTAL_OK);
 CHECK(fractal_numeric_binary64_v1.complex_construct(&r,&i,&result)==FRACTAL_OK);
 return result;
}

static void registry_and_assembly(void){
 fractal_module_registry installed,extended;
 fractal_module_registration entries[FRACTAL_REGISTRY_MAX_MODULES];
 fractal_module_descriptor incompatible_descriptor=*fractal_numeric_binary64_v1.descriptor;
 fractal_module_descriptor formula_descriptor=*fractal_formula_mandelbrot_v1.descriptor;
 fractal_numeric_vtable incompatible=fractal_numeric_binary64_v1;
 fractal_formula_vtable formula_fixture=fractal_formula_mandelbrot_v1;
 fractal_runtime_selection selected;
 fractal_runtime_modules runtime;
 fractal_memory_backend memory;
 size_t i;
 CHECK(fractal_installed_modules_registry(&installed)==FRACTAL_OK&&installed.count==22);
 CHECK(fractal_module_registry_count(&installed,FRACTAL_MODULE_FORMULA)==2);
 CHECK(fractal_module_registry_implementation(&installed,FRACTAL_MODULE_FORMULA,
  FRACTAL_FORMULA_MANDELBROT_V1_ID)==&fractal_formula_mandelbrot_v1);
 CHECK(fractal_module_registry_implementation(&installed,FRACTAL_MODULE_FORMULA,
  FRACTAL_FORMULA_JULIA_V1_ID)==&fractal_formula_julia_v1);
 CHECK(fractal_module_registry_find(&installed,FRACTAL_MODULE_FORMULA,
  FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID)==NULL);
 CHECK(fractal_memory_backend_init_system(&memory)==FRACTAL_OK);
 selected=selection("fractal.formula.unknown.v1",FRACTAL_NUMERIC_BINARY64_V1_ID);
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==
  FRACTAL_ERROR_NOT_IMPLEMENTED);
 selected=selection(FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID,
  FRACTAL_NUMERIC_BINARY64_V1_ID);
 CHECK(fractal_runtime_assemble(&installed,&selected,&memory,&runtime)==FRACTAL_OK&&
  runtime.formula==&fractal_formula_mandelbrot_v1);
 for(i=0;i<installed.count;i++)entries[i]=installed.entries[i];
 incompatible_descriptor.module_id="fractal.numeric.test.incompatible.v1";
 incompatible_descriptor.display_name="Incompatible numeric fixture";
 incompatible.descriptor=&incompatible_descriptor;
 incompatible.capability_flags&=~FRACTAL_NUMERIC_CAP_COMPLEX_SQUARE;
 entries[installed.count]=(fractal_module_registration){&incompatible_descriptor,&incompatible};
 CHECK(fractal_module_registry_init(&extended,entries,installed.count+1u)==FRACTAL_OK);
 selected=selection(FRACTAL_FORMULA_MANDELBROT_V1_ID,
  incompatible_descriptor.module_id);
 CHECK(fractal_runtime_assemble(&extended,&selected,&memory,&runtime)==
  FRACTAL_ERROR_INVALID_SPEC);
 for(i=0;i<installed.count;i++)entries[i]=installed.entries[i];
 formula_descriptor.module_id="fractal.formula.test.bad-interface.v1";
 formula_descriptor.display_name="Bad formula interface fixture";
 formula_fixture.descriptor=&formula_descriptor;
 formula_fixture.interface_version++;
 entries[installed.count]=(fractal_module_registration){&formula_descriptor,&formula_fixture};
 CHECK(fractal_module_registry_init(&extended,entries,installed.count+1u)==FRACTAL_OK);
 selected=selection(formula_descriptor.module_id,FRACTAL_NUMERIC_BINARY64_V1_ID);
 CHECK(fractal_runtime_assemble(&extended,&selected,&memory,&runtime)==
  FRACTAL_ERROR_INVALID_SPEC);
 formula_descriptor.available=false;
 formula_fixture=fractal_formula_mandelbrot_v1;
 formula_fixture.descriptor=&formula_descriptor;
 entries[installed.count]=(fractal_module_registration){&formula_descriptor,&formula_fixture};
 CHECK(fractal_module_registry_init(&extended,entries,installed.count+1u)==FRACTAL_OK);
 CHECK(fractal_runtime_assemble(&extended,&selected,&memory,&runtime)==
  FRACTAL_ERROR_NOT_IMPLEMENTED);
 fractal_memory_backend_shutdown(&memory);
}

static void contract_metadata(void){
 fractal_formula_vtable bad=fractal_formula_mandelbrot_v1;
 fractal_module_descriptor unavailable=*bad.descriptor;
 uint64_t mandelbrot_identity=fractal_formula_execution_identity_v1(
  &fractal_formula_mandelbrot_v1);
 uint64_t julia_identity=fractal_formula_execution_identity_v1(
  &fractal_formula_julia_v1);
 CHECK(sizeof(fractal_formula_state_v1)==FRACTAL_FORMULA_STATE_CAPACITY);
 CHECK(_Alignof(fractal_formula_state_v1)==FRACTAL_FORMULA_STATE_ALIGNMENT);
 CHECK(fractal_formula_validate_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1)==FRACTAL_OK);
 CHECK(fractal_formula_validate_v1(&fractal_formula_julia_v1,
  &fractal_numeric_binary64_v1)==FRACTAL_OK);
 CHECK(fractal_formula_mandelbrot_v1.interface_version==FRACTAL_FORMULA_INTERFACE_VERSION&&
  fractal_formula_mandelbrot_v1.contract_version==FRACTAL_FORMULA_CONTRACT_VERSION&&
  fractal_formula_mandelbrot_v1.parameter_size==sizeof(fractal_mandelbrot_parameters)&&
  fractal_formula_mandelbrot_v1.state_size<=FRACTAL_FORMULA_STATE_CAPACITY&&
  fractal_formula_mandelbrot_v1.state_alignment<=FRACTAL_FORMULA_STATE_ALIGNMENT);
 CHECK(mandelbrot_identity==UINT64_C(0x82001a07c687c7b1)&&
  julia_identity==UINT64_C(0xb05090f17f32fc19));
 CHECK(mandelbrot_identity==fractal_formula_execution_identity_v1(
  &fractal_formula_mandelbrot_v1));
 bad.interface_version++;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 bad=fractal_formula_mandelbrot_v1;bad.contract_version++;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 bad=fractal_formula_mandelbrot_v1;bad.parameter_size=FRACTAL_FORMULA_PARAMETER_CAPACITY+1u;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 bad=fractal_formula_mandelbrot_v1;bad.state_size=FRACTAL_FORMULA_STATE_CAPACITY+1u;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 bad=fractal_formula_mandelbrot_v1;bad.state_alignment=3u;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 unavailable.available=false;bad=fractal_formula_mandelbrot_v1;bad.descriptor=&unavailable;
 CHECK(fractal_formula_validate_v1(&bad,&fractal_numeric_binary64_v1)==
  FRACTAL_ERROR_INVALID_SPEC);
 printf("FORMULA execution mandelbrot=%016llx julia=%016llx state=%zu:%zu\n",
  (unsigned long long)mandelbrot_identity,(unsigned long long)julia_identity,
  fractal_formula_mandelbrot_v1.state_size,
  fractal_formula_mandelbrot_v1.state_alignment);
}

static void parameter_validation_and_identity(void){
 fractal_mandelbrot_parameters mandelbrot={2.0},changed_mandelbrot={3.0};
 fractal_julia_parameters julia={-0.8,0.156,2.0},changed_julia=julia;
 fractal_formula_parameters m={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &mandelbrot,sizeof(mandelbrot)};
 fractal_formula_parameters m_changed={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &changed_mandelbrot,sizeof(changed_mandelbrot)};
 fractal_formula_parameters m_compat={FRACTAL_FORMULA_MANDELBROT_COMPATIBILITY_ID,
  &mandelbrot,sizeof(mandelbrot)};
 fractal_formula_parameters j={FRACTAL_FORMULA_JULIA_V1_ID,&julia,sizeof(julia)};
 fractal_formula_parameters j_changed={FRACTAL_FORMULA_JULIA_V1_ID,
  &changed_julia,sizeof(changed_julia)};
 _Alignas(fractal_julia_parameters)
 unsigned char misaligned[sizeof(fractal_julia_parameters)+1u];
 fractal_formula_parameters bad_alignment={FRACTAL_FORMULA_JULIA_V1_ID,
  misaligned+1u,sizeof(fractal_julia_parameters)};
 uint64_t m1=0,m2=0,mc=0,j1=0,j2=0;
 CHECK(fractal_formula_parameters_validate_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m)==FRACTAL_OK);
 CHECK(fractal_formula_parameters_validate_v1(&fractal_formula_julia_v1,
  &fractal_numeric_binary64_v1,&j)==FRACTAL_OK);
 mandelbrot.escape_radius=0.0;
 CHECK(fractal_formula_parameters_validate_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m)==FRACTAL_ERROR_INVALID_SPEC);
 mandelbrot.escape_radius=-2.0;
 CHECK(fractal_formula_mandelbrot_v1.validate_parameters(&fractal_numeric_binary64_v1,
  &m)==FRACTAL_ERROR_INVALID_SPEC);
 mandelbrot.escape_radius=INFINITY;
 CHECK(fractal_formula_mandelbrot_v1.validate_parameters(&fractal_numeric_binary64_v1,
  &m)==FRACTAL_ERROR_INVALID_SPEC);
 mandelbrot.escape_radius=2.0;
 julia.constant_real=NAN;
 CHECK(fractal_formula_julia_v1.validate_parameters(&fractal_numeric_binary64_v1,&j)==
  FRACTAL_ERROR_INVALID_SPEC);
 julia.constant_real=-0.8;julia.constant_imaginary=INFINITY;
 CHECK(fractal_formula_julia_v1.validate_parameters(&fractal_numeric_binary64_v1,&j)==
  FRACTAL_ERROR_INVALID_SPEC);
 julia.constant_imaginary=0.156;julia.escape_radius=0.0;
 CHECK(fractal_formula_julia_v1.validate_parameters(&fractal_numeric_binary64_v1,&j)==
  FRACTAL_ERROR_INVALID_SPEC);
 julia.escape_radius=2.0;
 memcpy(misaligned+1u,&julia,sizeof(julia));
 CHECK(fractal_formula_parameters_validate_v1(&fractal_formula_julia_v1,
  &fractal_numeric_binary64_v1,&bad_alignment)==FRACTAL_ERROR_INVALID_SPEC);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m,&m1)==FRACTAL_OK);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m,&m2)==FRACTAL_OK&&m1==m2);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m_compat,&mc)==FRACTAL_OK&&mc==m1);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m_changed,&m2)==FRACTAL_OK&&m2!=m1);
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_julia_v1,
  &fractal_numeric_binary64_v1,&j,&j1)==FRACTAL_OK&&j1!=m1);
 changed_julia.constant_real=-0.7;
 CHECK(fractal_formula_parameter_identity_v1(&fractal_formula_julia_v1,
  &fractal_numeric_binary64_v1,&j_changed,&j2)==FRACTAL_OK&&j2!=j1);
 CHECK(m1==UINT64_C(0xbd014f20cbe74288)&&
  j1==UINT64_C(0x545eff8819ee50e0));
 printf("FORMULA parameters mandelbrot=%016llx changed=%016llx julia=%016llx changed=%016llx\n",
  (unsigned long long)m1,(unsigned long long)m2,(unsigned long long)j1,
  (unsigned long long)j2);
}

static void initialization_recurrence_and_classification(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_julia_parameters julia={0.0,0.0,2.0};
 fractal_julia_parameters threshold_low={0.25,-0.5,1.5};
 fractal_julia_parameters threshold_high={0.25,-0.5,1.6};
 fractal_formula_parameters m={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &mandelbrot,sizeof(mandelbrot)};
 fractal_formula_parameters j={FRACTAL_FORMULA_JULIA_V1_ID,&julia,sizeof(julia)};
 fractal_formula_parameters low={FRACTAL_FORMULA_JULIA_V1_ID,
  &threshold_low,sizeof(threshold_low)};
 fractal_formula_parameters high={FRACTAL_FORMULA_JULIA_V1_ID,
  &threshold_high,sizeof(threshold_high)};
 fractal_formula_state_v1 state;
 fractal_numeric_complex sample=complex_value(2.0,0.0);
 fractal_point_class classification=FRACTAL_CLASS_FAILED;
 fractal_point_result_compact point;
 bool terminated=true;
 CHECK(fractal_formula_mandelbrot_v1.initialize_state(&fractal_numeric_binary64_v1,
  &m,&sample,&state)==FRACTAL_OK);
 CHECK(fractal_formula_mandelbrot_v1.terminate(&fractal_numeric_binary64_v1,&state,
  &terminated)==FRACTAL_OK&&!terminated);
 CHECK(fractal_formula_mandelbrot_v1.recurrence(&fractal_numeric_binary64_v1,&state)==
  FRACTAL_OK);
 CHECK(fractal_formula_mandelbrot_v1.terminate(&fractal_numeric_binary64_v1,&state,
  &terminated)==FRACTAL_OK&&!terminated);
 CHECK(fractal_formula_mandelbrot_v1.recurrence(&fractal_numeric_binary64_v1,&state)==
  FRACTAL_OK);
 CHECK(fractal_formula_mandelbrot_v1.terminate(&fractal_numeric_binary64_v1,&state,
  &terminated)==FRACTAL_OK&&terminated);
 CHECK(fractal_formula_mandelbrot_v1.classify(&fractal_numeric_binary64_v1,&state,
  false,&classification)==FRACTAL_OK&&classification==FRACTAL_CLASS_ESCAPED);
 CHECK(fractal_formula_julia_v1.initialize_state(&fractal_numeric_binary64_v1,
  &j,&sample,&state)==FRACTAL_OK);
 CHECK(fractal_formula_julia_v1.terminate(&fractal_numeric_binary64_v1,&state,
  &terminated)==FRACTAL_OK&&!terminated);
 CHECK(fractal_formula_julia_v1.recurrence(&fractal_numeric_binary64_v1,&state)==
  FRACTAL_OK);
 CHECK(fractal_formula_julia_v1.terminate(&fractal_numeric_binary64_v1,&state,
  &terminated)==FRACTAL_OK&&terminated);
 sample=complex_value(1.0,1.0);
 CHECK(fractal_formula_julia_v1.initialize_state(&fractal_numeric_binary64_v1,
  &low,&sample,&state)==FRACTAL_OK&&
  fractal_formula_julia_v1.recurrence(&fractal_numeric_binary64_v1,&state)==FRACTAL_OK&&
  fractal_formula_julia_v1.terminate(&fractal_numeric_binary64_v1,&state,&terminated)==
   FRACTAL_OK&&terminated);
 CHECK(fractal_formula_julia_v1.initialize_state(&fractal_numeric_binary64_v1,
  &high,&sample,&state)==FRACTAL_OK&&
  fractal_formula_julia_v1.recurrence(&fractal_numeric_binary64_v1,&state)==FRACTAL_OK&&
  fractal_formula_julia_v1.terminate(&fractal_numeric_binary64_v1,&state,&terminated)==
   FRACTAL_OK&&!terminated);
 CHECK(fractal_compute_scalar_v1.point(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m,2.0,0.0,2,NULL,&point)==FRACTAL_OK&&
  point.steps==2&&point.classification==FRACTAL_CLASS_ESCAPED);
 CHECK(fractal_compute_scalar_v1.point(&fractal_formula_mandelbrot_v1,
  &fractal_numeric_binary64_v1,&m,0.0,0.0,1,NULL,&point)==FRACTAL_OK&&
  point.steps==1&&point.classification==FRACTAL_CLASS_UNRESOLVED);
}

static fractal_result fail_initialize(const fractal_numeric_vtable *numeric,
 const fractal_formula_parameters *parameters,const fractal_numeric_complex *sample,
 fractal_formula_state_v1 *state){
 (void)numeric;(void)parameters;(void)sample;(void)state;
 return FRACTAL_ERROR_IO;
}
static fractal_result fail_recurrence(const fractal_numeric_vtable *numeric,
 fractal_formula_state_v1 *state){(void)numeric;(void)state;return FRACTAL_ERROR_IO;}
static fractal_result fail_termination(const fractal_numeric_vtable *numeric,
 fractal_formula_state_v1 *state,bool *terminated){
 (void)numeric;(void)state;(void)terminated;return FRACTAL_ERROR_IO;
}
static fractal_result fail_classification(const fractal_numeric_vtable *numeric,
 const fractal_formula_state_v1 *state,bool exhausted,fractal_point_class *classification){
 (void)numeric;(void)state;(void)exhausted;(void)classification;return FRACTAL_ERROR_IO;
}

static void failure_semantics(void){
 fractal_mandelbrot_parameters mandelbrot={2.0};
 fractal_formula_parameters parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &mandelbrot,sizeof(mandelbrot)};
 fractal_formula_vtable formula=fractal_formula_mandelbrot_v1;
 fractal_point_result_compact point;
 formula.initialize_state=fail_initialize;
 CHECK(fractal_compute_scalar_v1.point(&formula,&fractal_numeric_binary64_v1,
  &parameters,2.0,0.0,2,NULL,&point)==FRACTAL_ERROR_INVALID_SPEC&&
  point.classification==FRACTAL_CLASS_UNRESOLVED);
 formula=fractal_formula_mandelbrot_v1;formula.recurrence=fail_recurrence;
 CHECK(fractal_compute_scalar_v1.point(&formula,&fractal_numeric_binary64_v1,
  &parameters,2.0,0.0,2,NULL,&point)==FRACTAL_OK&&
  point.classification==FRACTAL_CLASS_FAILED);
 formula=fractal_formula_mandelbrot_v1;formula.terminate=fail_termination;
 CHECK(fractal_compute_scalar_v1.point(&formula,&fractal_numeric_binary64_v1,
  &parameters,2.0,0.0,2,NULL,&point)==FRACTAL_ERROR_INVALID_SPEC&&
  point.classification==FRACTAL_CLASS_FAILED);
 formula=fractal_formula_mandelbrot_v1;formula.classify=fail_classification;
 CHECK(fractal_compute_scalar_v1.point(&formula,&fractal_numeric_binary64_v1,
  &parameters,2.0,0.0,2,NULL,&point)==FRACTAL_ERROR_INVALID_SPEC&&
  point.classification==FRACTAL_CLASS_FAILED);
}

static void work_unit_parameter_sensitivity(void){
 fractal_mandelbrot_parameters first_value={2.0},second_value={2.5};
 fractal_formula_parameters first_parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &first_value,sizeof(first_value)};
 fractal_formula_parameters second_parameters={FRACTAL_FORMULA_MANDELBROT_V1_ID,
  &second_value,sizeof(second_value)};
 fractal_job_spec first_job={{FRACTAL_FORMULA_MANDELBROT_V1_ID,first_parameters,64},
  {-0.5,0.0,3.0,4,4},{"palette.socket-v1",FRACTAL_PIXEL_BGR8},
  {"encoder.bmp.v3","formula-first.bmp"}};
 fractal_job_spec second_job=first_job;
 fractal_runtime_modules runtime={0};
 fractal_field_descriptor field={4,4,4*sizeof(fractal_point_result_compact),
  FRACTAL_FIELD_ITERATION_CLASSIFICATION_V1,0};
 fractal_computation_problem_v1 first_problem,second_problem;
 fractal_sealed_work_unit_v1 first_unit,second_unit;
 size_t count=0;
 second_job.problem.parameters=second_parameters;
 runtime.formula=&fractal_formula_mandelbrot_v1;
 runtime.numeric=&fractal_numeric_binary64_v1;
 runtime.compute=&fractal_compute_scalar_v1;
 CHECK(fractal_computation_problem_init_v1(&runtime,&first_job,&field,&first_problem)==
  FRACTAL_OK);
 CHECK(fractal_computation_problem_init_v1(&runtime,&second_job,&field,&second_problem)==
  FRACTAL_OK);
 CHECK(first_problem.formula_parameter_identity!=second_problem.formula_parameter_identity&&
  first_problem.formula_execution_identity==second_problem.formula_execution_identity&&
  first_problem.identity!=second_problem.identity);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,
  &first_problem,1,FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,&first_unit,1,&count)==
  FRACTAL_OK&&count==1);
 CHECK(fractal_scheduler_decompose_computation_v1(&fractal_compute_scalar_v1,
  &second_problem,1,FRACTAL_COMPUTATION_CANCEL_POINT_ITERATION,&second_unit,1,&count)==
  FRACTAL_OK&&count==1);
 CHECK(first_unit.identity!=second_unit.identity&&
  first_unit.formula_parameter_identity!=second_unit.formula_parameter_identity&&
  !strcmp(first_unit.formula_id,FRACTAL_FORMULA_MANDELBROT_V1_ID));
}

int main(void){
 registry_and_assembly();
 contract_metadata();
 parameter_validation_and_identity();
 initialization_recurrence_and_classification();
 failure_semantics();
 work_unit_parameter_sensitivity();
 return failures?1:0;
}
