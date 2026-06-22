# Mini-Lisp 解释器

一个基于 C++20 的 Scheme/Lisp 子集解释器，支持交互式 REPL 和文件执行模式。

## 基本功能

- **数值运算**：`+` `-` `*` `/` `abs` `expt` `quotient` `remainder` `modulo`
- **比较运算**：`=` `>` `<` `>=` `<=` `even?` `odd?` `zero?`
- **逻辑运算**：`and` `or` `not`
- **列表操作**：`car` `cdr` `cons` `list` `append` `length`
- **高阶函数**：`map` `filter` `reduce` `apply`
- **特殊形式**：`define` `lambda` `if` `cond` `begin` `let` `quote` `quasiquote`
- **类型检查**：`atom?` `boolean?` `integer?` `number?` `list?` `null?` `pair?` `procedure?` `string?` `symbol?`
- **其他**：`print` `display` `displayln` `eval` `exit` `newline` `error`

## 编译

### Visual Studio（推荐）

打开 `mini-lisp.sln`，按 `Ctrl+Shift+B` 编译。

### MinGW / g++

```bash
g++ -std=c++20 -O2 \
  -I lib/replxx/include -I lib/replxx/src \
  -DREPLXX_STATIC \
  builtin.cpp eval_env.cpp forms.cpp main.cpp parser.cpp \
  REPL_Refiner.cpp token.cpp tokenizer.cpp value.cpp \
  lib/replxx/src/replxx.cxx lib/replxx/src/replxx_impl.cxx \
  lib/replxx/src/conversion.cxx lib/replxx/src/escape.cxx \
  lib/replxx/src/history.cxx lib/replxx/src/prompt.cxx \
  lib/replxx/src/terminal.cxx lib/replxx/src/util.cxx \
  lib/replxx/src/windows.cxx \
  lib/replxx/src/ConvertUTF.cpp lib/replxx/src/wcwidth.cpp \
  -o mini-lisp.exe -lws2_32
```

## 使用方法

### 交互式 REPL

```bash
./mini-lisp
```

### 执行脚本文件

```bash
./mini-lisp path/to/file.scm
```

## REPL 功能

交互式环境下支持以下功能：

### 多行输入

自动检测括号/字符串是否闭合，未闭合时显示 `... ` 续行提示：

```
>>> (define (fib n)
...   		(if (<= n 1)
...       		n
...       		(+ (fib (- n 1)) (fib (- n 2)))))
()
```

### 自动缩进

续行时光标自动对齐到第一个参数列，或缩进 2 格。

### 语法高亮

输入时关键字、内置函数、数字、字符串、括号等以不同颜色区分。

### Tab 补全

按 Tab 自动补全内置函数名和特殊形式（`define`、`lambda`、`car`、`cdr` 等）。

### 输出着色

求值结果根据类型显示不同颜色：
- 数字 → 绿色
- 字符串 → 红色
- 布尔值 → 黄色
- 符号 → 青色
- 过程 → 品红
- 空列表 → 灰色

### 快捷键

| 按键 | 功能 |
|------|------|
| 方向键 | 移动光标 |
| Ctrl+D | 退出 REPL |
| Tab | 补全 |
