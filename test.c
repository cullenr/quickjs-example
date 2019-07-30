/**
 *  Call the fib function and pass a callback to receive the response.
 *
 *
 * import { test } from "./test.so";
 * test(10, res => console.log(res)); // 55
 *
 */

#include "quickjs/quickjs.h"

#define countof(x) (sizeof(x) / sizeof((x)[0]))

static JSValue js_print(JSContext *ctx, JSValueConst this_val,
                              int argc, JSValueConst *argv)
{
    int i;
    const char *str;

    for(i = 0; i < argc; i++) {
        if (i != 0)
            putchar(' ');
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        fputs(str, stdout);
        JS_FreeCString(ctx, str);
    }
    putchar('\n');
    return JS_UNDEFINED;
}
/**
 *  Copied from quickjs-libc.c
 */
static void js_std_dump_error(JSContext *ctx)
{
    JSValue exception_val, val;
    const char *stack;
    JS_BOOL is_error;
    
    exception_val = JS_GetException(ctx);
    is_error = JS_IsError(ctx, exception_val);
    if (!is_error)
        printf("Throw: ");
    js_print(ctx, JS_NULL, 1, (JSValueConst *)&exception_val);
    if (is_error) {
        val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            stack = JS_ToCString(ctx, val);
            printf("%s\n", stack);
            JS_FreeCString(ctx, stack);
        }
        JS_FreeValue(ctx, val);
    }
    JS_FreeValue(ctx, exception_val);
}

static int fib(int n)
{
    if (n <= 0)
        return 0;
    else if (n == 1)
        return 1;
    else
        return fib(n - 1) + fib(n - 2);
}

static JSValue js_fib(JSContext *ctx, JSValueConst this_val,
                      int argc, JSValueConst *argv)
{
    int c_in, c_out;
    JSValue js_out, res;
    JSValueConst func;

    if (JS_ToInt32(ctx, &c_in, argv[0]))
        return JS_EXCEPTION;

    func = argv[1];

    if (!JS_IsFunction(ctx, func))
        return JS_ThrowTypeError(ctx, "not a function");

    c_out   = fib(c_in);
    js_out  = JS_NewInt32(ctx, c_out);
    res     = JS_Call(ctx, func, JS_UNDEFINED, 1, &js_out);

    if (JS_IsException(res))
        js_std_dump_error(ctx);

    JS_FreeValue(ctx, res);
    JS_FreeValue(ctx, js_out);

    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_fib_funcs[] = {
    JS_CFUNC_DEF("test", 2, js_fib ),
};

static int js_fib_init(JSContext *ctx, JSModuleDef *m)
{
    return JS_SetModuleExportList(ctx, m, js_fib_funcs,
                                  countof(js_fib_funcs));
}

#ifdef JS_SHARED_LIBRARY
#define JS_INIT_MODULE js_init_module
#else
#define JS_INIT_MODULE js_init_module_test
#endif

JSModuleDef *JS_INIT_MODULE(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_fib_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_fib_funcs, countof(js_fib_funcs));
    return m;
}
