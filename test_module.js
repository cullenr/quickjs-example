/* example of JS module importing a C module */

import { test } from "./test.so";

function callback(res) {
    console.log('hello');
    console.log(res);
}

test(10, callback);
