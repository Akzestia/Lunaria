## Todo 

# Building Docker Image

### Clone the repository

```git
git clone https://github.com/Akzestia/Lunaria.git
cd Lunaria
```

Get client submodule by running this command ```git submodule update --init --recursive```

### Build Image

```sh
docker build -t lunaria-server .
```

### Run the Docker Container

```sh
docker run -d --name lunaria-container lunaria-server
```
