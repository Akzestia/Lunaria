<h3>Creating tables with run.scripts.sh</h3>


### Postgress
  - Linux
    ```
    chmod +x run.scripts.sh
    ./run.scripts.sh
    ```
  - Windows (Git Bash)
    ```
    chmod +x run.scripts.sh
    ./run.scripts.sh
    ```

### Scylla
  - Linux (default Scylla username && password)
    ```
    chmod +x run.scripts.cql.sh
    ./run.scripts.cql.sh -u casandra -p casandra -s ../Scylla/cql
    ```
 - Linux (Replace user_name && password with your username && password)
    ```
    ./run.scripts.cql.sh -u <user_name> -p <password> -s ../Scylla/cql 
    ```
