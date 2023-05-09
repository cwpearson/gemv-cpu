# CPU Small GEMV performance

```
mkdir build && cd build
cmake ..
make && ./main
```

## M1 Mac Air

```
-------------------------------------------------------------------------------
Benchmark                     Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------
Naive/4                  626655 ns       626639 ns         1119 bytes_per_second=33.2913G/s
Naive/5                  920622 ns       920617 ns          766 bytes_per_second=32.3721G/s
Naive/6                 1340142 ns      1340126 ns          522 bytes_per_second=30.0219G/s
Naive/7                 1833808 ns      1833762 ns          383 bytes_per_second=28.441G/s
Naive/8                 1450496 ns      1450255 ns          490 bytes_per_second=45.2094G/s
Naive/9                 1884987 ns      1884968 ns          371 bytes_per_second=42.6884G/s
Naive/10                2268544 ns      2268547 ns          309 bytes_per_second=42.6959G/s
Naive/11                2851308 ns      2851293 ns          246 bytes_per_second=40.241G/s
Naive/12                3389824 ns      3389821 ns          207 bytes_per_second=39.5627G/s
Naive/13                4182271 ns      4182280 ns          168 bytes_per_second=37.0545G/s
BlockedM/4               696059 ns       696052 ns         1007 bytes_per_second=29.9714G/s
BlockedM/5               866819 ns       866819 ns          807 bytes_per_second=34.3812G/s
BlockedM/6              1150716 ns      1150716 ns          609 bytes_per_second=34.9636G/s
BlockedM/7              1370344 ns      1370329 ns          508 bytes_per_second=38.0595G/s
BlockedM/8              1299355 ns      1299354 ns          537 bytes_per_second=50.4598G/s
BlockedM/9              1612277 ns      1612256 ns          437 bytes_per_second=49.9091G/s
BlockedM/10             1889108 ns      1889111 ns          369 bytes_per_second=51.2715G/s
BlockedM/11             2364830 ns      2364831 ns          296 bytes_per_second=48.5189G/s
BlockedM/12             2715897 ns      2715764 ns          258 bytes_per_second=49.3822G/s
BlockedM/13             3153739 ns      3153740 ns          223 bytes_per_second=49.1391G/s
BlockedM_Align128/4      696069 ns       696068 ns         1003 bytes_per_second=29.9707G/s
BlockedM_Align128/5      887254 ns       887255 ns          789 bytes_per_second=33.5894G/s
BlockedM_Align128/6     1175957 ns      1175951 ns          594 bytes_per_second=34.2133G/s
BlockedM_Align128/7     1402163 ns      1402165 ns          498 bytes_per_second=37.1954G/s
BlockedM_Align128/8     1286006 ns      1286007 ns          544 bytes_per_second=50.9835G/s
BlockedM_Align128/9     1677648 ns      1677645 ns          403 bytes_per_second=47.9638G/s
BlockedM_Align128/10    1970952 ns      1970955 ns          357 bytes_per_second=49.1424G/s
BlockedM_Align128/11    2397129 ns      2397099 ns          293 bytes_per_second=47.8658G/s
BlockedM_Align128/12    2725270 ns      2725262 ns          252 bytes_per_second=49.2101G/s
BlockedM_Align128/13    3191477 ns      3191394 ns          221 bytes_per_second=48.5594G/s
BlockedN/4               628948 ns       628948 ns         1111 bytes_per_second=33.1691G/s
BlockedN/5               922175 ns       922177 ns          759 bytes_per_second=32.3174G/s
BlockedN/6              1332731 ns      1332701 ns          525 bytes_per_second=30.1892G/s
BlockedN/7              1844790 ns      1844792 ns          380 bytes_per_second=28.271G/s
BlockedN/8              2719916 ns      2719915 ns          258 bytes_per_second=24.1056G/s
BlockedN/9              3501141 ns      3501130 ns          200 bytes_per_second=22.9829G/s
BlockedN/10             4170226 ns      4170232 ns          168 bytes_per_second=23.2259G/s
BlockedN/11             4880665 ns      4880625 ns          144 bytes_per_second=23.5091G/s
BlockedN/12             5639581 ns      5639589 ns          124 bytes_per_second=23.7802G/s
BlockedN/13             6472200 ns      6472213 ns          108 bytes_per_second=23.9442G/s
```