#include <util.h>

void gen_rand_no_entropy(void *output, size_t output_len)
{
   FILE *f;
   size_t rnd_sz, left;

   if (!(f=fopen("/dev/urandom", "r")))
      return;

   rnd_sz=0;
   left=output_len;

   while ((rnd_sz+=fread(output+rnd_sz, 1, left, f))<output_len)
      left-=rnd_sz;

   fclose(f);

   return;

}

int mynanojs_add_nano_function_util(napi_env env, napi_value exports, void *handle)
{

   napi_value fn;
   MY_NANO_JS_FUNCTION *function=(MY_NANO_JS_FUNCTION *)handle;

   while (function->function_name) {

      if (napi_create_function(env, NULL, 0, function->fn, NULL, &fn)!=napi_ok)
         return 300;

      if (napi_set_named_property(env, exports, function->function_name, fn)!=napi_ok)
         return 301;

      function++;

   }

   return 0;

}

