#include <stdio.h>
#include <math.h>

#include "wasmer.h"

int main() {
    const char *wat_string =
        "(module\n"
        "  (func $main (param $value f64) (result f64)\n"
        "     local.get $value\n"
        "     f64.sqrt)\n"
        "  (export \"main\" (func $main)))";
    wasm_byte_vec_t wat;
    wasm_byte_vec_new(&wat, strlen(wat_string), wat_string);
    wasm_byte_vec_t wasm_bytes;
    wat2wasm(&wat, &wasm_bytes);
    wasm_byte_vec_delete(&wat);

    wasm_config_t* config = wasm_config_new();
    wasm_config_set_compiler(config, SINGLEPASS);

    wasm_engine_t* engine = wasm_engine_new_with_config(config);
    wasm_store_t* store = wasm_store_new(engine);

    wasm_module_t* module = wasm_module_new(store, &wasm_bytes);
    assert(module != NULL);

    wasm_byte_vec_delete(&wasm_bytes);

    wasm_extern_vec_t imports = WASM_EMPTY_VEC;

    wasm_instance_t* instance = wasm_instance_new(store, module, &imports, NULL);
    assert(instance != NULL);

    wasm_extern_vec_t exports;
    wasm_instance_exports(instance, &exports);

    assert(exports.size > 0);

    wasm_func_t* main_func = wasm_extern_as_func(exports.data[0]);
    assert(main_func != NULL);

    wasm_module_delete(module);
    wasm_instance_delete(instance);

    wasm_val_t args_val[1] = { WASM_F64_VAL(-10.0) };
    wasm_val_t results_val[1] = { WASM_INIT_VAL };

    wasm_val_vec_t args = WASM_ARRAY_VEC(args_val);
    wasm_val_vec_t results = WASM_ARRAY_VEC(results_val);

    assert(wasm_func_call(main_func, &args, &results) == 0);

    double native_nan = sqrt(-10.0);
    long native_nan_value = *((long *) &native_nan);
    printf("Native NaN: %lx\n", native_nan_value);

    long wasm_value = *((long *) &results_val[0].of.f64);
    printf("Results: %f - %lx\n", results_val[0].of.f64, wasm_value);

    wasm_extern_vec_delete(&exports);
    wasm_store_delete(store);
    wasm_engine_delete(engine);

    return 0;
}
