/* example of JS module importing a C module */

import { test } from "./test.so";

console.log("Hello World");
console.log("test(10)=", test(10));
