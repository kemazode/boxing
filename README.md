### $ boxing --help
```
Usage: boxing [OPTIONS] [FILE]
boxing is a simple program helps you to create easy-to-read lists, grouped in blocks

  -h, --help                 Print help and exit
  -V, --version              Print version and exit
  -b, --box=<box>            Specify box
  -c, --create               Create new box
  -a, --append=<b1>,<b2>...  Append given entries to the specified box
  -r, --read                 Read the box
  -d, --delete               Delete the box and all entries contained in
  -l, --list                 List all existing boxes

License: GPLv3+: GNU GPL version 3 or later.
This is free software; see the source for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Written by yachmenka <yachmenka.git@gmail.com>
```

### Example
$ cat words.list
```
impose
fabricate
interact
bunch
possess
properly
whatever
facilitate
clap
avenger
wild
extensive
convinced
cause
insipre
```
$ boxing -ca "$(cat words.list | tr '\n' ',')" -b "$(date "+%D")" words  
$ cat words
```
 11/09/19
+------------+
| impose     |
| fabricate  |
| interact   |
| bunch      |
| possess    |
| properly   |
| whatever   |
| facilitate |
| clap       |
| avenger    |
| wild       |
| extensive  |
| convinced  |
| cause      |
| insipre    |
+------------+
```
