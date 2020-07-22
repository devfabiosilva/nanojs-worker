#define NAPI_EXPERIMENTAL
#include <node_api.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define F_DEFAULT_THRESHOLD (uint64_t)0xffffffc000000000

typedef napi_value (*my_nano_fn)(napi_env, napi_callback_info);
typedef void (*rnd_fn)(void *, size_t);
typedef struct my_nano_js_fn_call_t {
   const char *function_name;
   my_nano_fn fn;
} MY_NANO_JS_FUNCTION;

void f_random_detach();
void f_random(void *, size_t);
void f_random_attach(rnd_fn);
void gen_rand_no_entropy(void *, size_t);
void *f_is_random_attached();
int f_str_to_hex(uint8_t *,char *);
int f_nano_pow(uint64_t *PoW_res, unsigned char *, const uint64_t, int);
double to_multiplier(uint64_t, uint64_t);
uint64_t from_multiplier(double, uint64_t);

int mynanojs_add_nano_function_util(napi_env, napi_value, void *);

#define CANT_PARSE_JAVASCRIPT_ARGS "Can't parse arguments"
#define PARSE_ERROR "101"
#define ERROR_TOO_MANY_ARGUMENTS "Too many arguments"
#define ERROR_MISSING_ARGS "Missing arguments"
#define ERROR_PARSING_BASE_DIFFICULTY_MSG "Error when parsing base diffuculty to big int"
#define ERROR_PARSING_BASE_DIFFICULTY "543"
#define ERROR_PRECISION_LOSS_DIFFICULTY_MSG "Precision loss in base difficulty big int"
#define ERROR_PRECISION_LOSS_DIFFICULTY "544"
#define NANOJS_NAPI_INIT_ERROR "myNanoJS internal error in C function \"%s\" %s"

