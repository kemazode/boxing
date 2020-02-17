### Example
$ cat words
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
