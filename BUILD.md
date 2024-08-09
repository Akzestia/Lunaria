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

### Build Image
```sh
docker build -t lunaria-server .
```

### Run the Docker Container
```sh
docker run -d --name lunaria-container lunaria-server
```

**Reminder: As mentioned above, these build instructions are provided for reference only. The project is still in development, and building/running it is not advised at this stage.**
