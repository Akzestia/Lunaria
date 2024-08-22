# BUILD.md

**Note: This project is currently under active development. Building and running the project is not recommended at this time. Please check back later for updates.**

## Todo 

# Building Docker Image

### Clone the repository
```git
git clone https://github.com/Akzestia/Lunaria.git
cd Lunaria
```

Get client submodule by running this command:
```
git submodule update --init --recursive
```

Create config.ini file inside db/config.ini
```
DB_HOST=
DB_USER=
DB_NAME=
DB_PASSWORD=

SCYLLA_HOST=
SCYLLA_PORT=
SCYLLA_KEYSPACE=lunnaria_service
SCYLLA_USER=
SCYLLA_PASSWORD=
```
> Replace with your values. DB_ is used for psql, and SCYLLA_ for scylladb. If u didn't modify any of cql scripts, don't change the KEYSPACE. 

### Build Image
```sh
docker build -t lunaria-server .
```

### Run the Docker Container
```sh
docker run -d --name lunaria-container lunaria-server
```

**Reminder: As mentioned above, these build instructions are provided for reference only. The project is still in development, and building/running it is not advised at this stage.**
