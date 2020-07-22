#include <util.h>

#define F_BUF_CHAR (size_t)512
static char _buf[F_BUF_CHAR];

napi_value nanojs_from_multiplier(napi_env env, napi_callback_info info)
{
   napi_value argv[2], res;
   size_t argc=2;
   double multiplier;
   uint64_t base_difficulty;
   bool lossless;

// multiplier: double, base_difficulty (optional)

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (napi_get_value_double(env, argv[0], &multiplier)!=napi_ok) {
      napi_throw_error(env, "542", "Can't parse double multiplier");
      return NULL;
   }

   if (multiplier<=0.0) {
      napi_throw_error(env, "546", "Invalid multiplier");
      return NULL;
   }

   if (argc==1)
      base_difficulty=F_DEFAULT_THRESHOLD;
   else {
      if (napi_get_value_bigint_uint64(env, argv[1], &base_difficulty, &lossless)!=napi_ok) {
         napi_throw_error(env, ERROR_PARSING_BASE_DIFFICULTY, ERROR_PARSING_BASE_DIFFICULTY_MSG);
         return NULL;
      }

      if (!lossless) {
         napi_throw_error(env, ERROR_PRECISION_LOSS_DIFFICULTY, ERROR_PRECISION_LOSS_DIFFICULTY_MSG);
         return NULL;
      }
   }

   if (napi_create_bigint_uint64(env, from_multiplier(multiplier, base_difficulty), &res)!=napi_ok) {
      napi_throw_error(env, "545", "Can't create Big Int from multiplier");
      res=NULL;
   }

   return res;
}

napi_value nanojs_to_multiplier(napi_env env, napi_callback_info info)
{
   napi_value argv[2], res;
   size_t argc=2;
   uint64_t difficulty, base_difficulty;
   bool lossless;

//difficulty: uint64_t, base_difficulty: uint64_t (optional)

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc>2) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (!argc) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc==1)
      base_difficulty=F_DEFAULT_THRESHOLD;
   else {
      if (napi_get_value_bigint_uint64(env, argv[1], &base_difficulty, &lossless)!=napi_ok) {
         napi_throw_error(env, ERROR_PARSING_BASE_DIFFICULTY, ERROR_PARSING_BASE_DIFFICULTY_MSG);
         return NULL;
      }

      if (!lossless) {
         napi_throw_error(env, ERROR_PRECISION_LOSS_DIFFICULTY, ERROR_PRECISION_LOSS_DIFFICULTY_MSG);
         return NULL;
      }
   }

   if (napi_get_value_bigint_uint64(env, argv[0], &difficulty, &lossless)!=napi_ok) {
      napi_throw_error(env, "547", "Can't parse Big Int difficulty");
      return NULL;
   }

   if (!lossless) {
      napi_throw_error(env, "548", "Precision loss in difficulty big int");
      return NULL;
   }

   if (napi_create_double(env, to_multiplier(difficulty, base_difficulty), &res)!=napi_ok) {
      napi_throw_error(env, "549", "Can't create double result from multiplier");
      res=NULL;
   }

   return res;
}

napi_value nanojs_pow(napi_env env, napi_callback_info info)
{
   int err;
   napi_value argv[3], res;
   size_t argc=3, sz_tmp;
   uint64_t threshold;
   int32_t n_thr;
   bool lossless;

   if (napi_get_cb_info(env, info, &argc, &argv[0], NULL, NULL)!=napi_ok) {
      napi_throw_error(env, PARSE_ERROR, CANT_PARSE_JAVASCRIPT_ARGS);
      return NULL;
   }

   if (argc<2) {
      napi_throw_error(env, NULL, ERROR_MISSING_ARGS);
      return NULL;
   }

   if (argc>3) {
      napi_throw_error(env, NULL, ERROR_TOO_MANY_ARGUMENTS);
      return NULL;
   }

   if (argc==2)
      threshold=F_DEFAULT_THRESHOLD;
   else {
      if (napi_get_value_bigint_uint64(env, argv[2], &threshold, &lossless)!=napi_ok) {
         napi_throw_error(env, "124", "Error when parsing PoW threshold");
         return NULL;
      }

      if (!lossless) {
         napi_throw_error(env, "125", "Precision loss in threshold big int");
         return NULL;
      }
   }

   if (napi_get_value_int32(env, argv[1], &n_thr)!=napi_ok) {
      napi_throw_error(env, "126", "Error when parsing CPU number of threads");
      return NULL;
   }

   if (napi_get_value_string_utf8(env, argv[0], _buf, sizeof(_buf), &sz_tmp)!=napi_ok) {
      napi_throw_error(env, "127", "Can't parse Nano Big Number B value");
      return NULL;
   }

   if (sz_tmp!=64) {
      sprintf(_buf, "Wrong hash size %lu to calculate PoW", (unsigned long int)sz_tmp);
      napi_throw_error(env, "128", _buf);
      return NULL;
   }

   _buf[64]=0;

   if ((err=f_str_to_hex((uint8_t *)(_buf+128), _buf))) {
      sprintf((_buf+128), "Can't convert hash \"%s\" to binary", _buf);
      napi_throw_error(env, "129", (_buf+128));
      return NULL;
   }

   f_random_attach(gen_rand_no_entropy);

   if ((err=f_nano_pow((uint64_t *)_buf, (unsigned char *)(_buf+128), (const uint64_t)threshold, (int)n_thr))) {
      sprintf(_buf, "%d", err);
      napi_throw_error(env, _buf, "Internal error in myNanoEmbedded C function 'f_nano_pow'");
   }

   f_random_detach();

   if (err)
      return NULL;

   if (napi_create_bigint_uint64(env, (uint64_t)*((uint64_t *)_buf), &res)!=napi_ok) {
      napi_throw_error(env, "130", "Can't parse result threshold JavaScript environment");
      return NULL;
   }

   return res;
}

MY_NANO_JS_FUNCTION NANO_JS_FUNCTIONS[] = {

   {"nanojs_from_multiplier", nanojs_from_multiplier},
   {"nanojs_to_multiplier", nanojs_to_multiplier},
   {"nanojs_pow", nanojs_pow},
   {NULL, NULL}

};

napi_value Init(napi_env env, napi_value exports)
{
   int err;

   if ((err=mynanojs_add_nano_function_util(env, exports, NANO_JS_FUNCTIONS))) {
      sprintf(_buf, "%d", err);
      sprintf(_buf+128, NANOJS_NAPI_INIT_ERROR, "mynanojs_add_nano_function_util", _buf);
      napi_throw_error(env, _buf, _buf+128);
      return NULL;
   }

   return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

