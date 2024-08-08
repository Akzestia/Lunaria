## Using scripts in psql

Connecting to psql
```
  psql -U username -d database
```
Executing scripts with psql
```
  psql -U username -d database -f /path/to/script/script_name.sql
```
Executing scripts with \i
```
  \i /path/to/script/script_name.sql
```
