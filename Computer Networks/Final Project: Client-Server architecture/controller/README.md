# Controller sotfware

_Made by Rémi Blaise & Achraf El Khamsi_

####################
## How to compile ##
####################

in the 'controller' folder :

```sh

make

```
#################
### How to use ##
#################

Execute to launch server:
```bash

./build/server

```

You can also give the port number if you want to override the config value:
```bash

./build/server 3001

```

###################################
## Test of command line commands ##
###################################

Then enter commands, for example:

```bash

load data.in
add view N1 9x9+9+9
add view N1 2x2+2+2
add view N2 10x0+0+0
show
del view N1
show
save data.out

```

## How to log

You can give `-v` as last command line param to set logger to VERBOSE. Logger is not verbose by default.
Logging is written is `log` file.

######################################################
## Test of the server/client communication protocol ##
######################################################

we will use a fake client to test this communication protocol

First, start the server :

```bash

./build/server 30000
```

then start a client and link it to the server port (in this case port 30000)

```bash

./build/manual_client localhost 30000
```

In the client command line, we can enter the following commands :
( > refers to a sent message from the client)
( < refers to the expected answer from the server )
( #... refers to informative comments that sould not be included in the command )
( ? refers to a value that depends on the state of the server )

### Hello protocol

> hello
< no greeting  #if the server have not loaded an aquarium yet

> hello
< greeting N?  #the server returns the first available view

> hello in as N3
< greeting N3  #if the N3 view is existing and free

> hello in as N404
< greeting N?  #the server returns the first available view

> hello
< You are already connected to a view  #if the client have already connected to a view


### add / del / start Fish protocol

> addFish PoissonRouge at 50x50, 2x2, RandomWayPoint
< OK

> addFish PoissonRouge
< NOK   # the syntax is not correct

> addFish PoissonRouge at 50x50, 2x2, RandomWayPoint
< NOK : le poisson existe déja  #if PoissonRouge already exists

> delFish PoissonRouge
< OK

> delFish PoissonPirate
< NOK : poisson inexistant

> startFish PoissonRouge
< OK

> startFish PoissonPirate
< NOK : poisson inexistant


### getFishes / getFishesContinously protocol

> getFishes
< Vous n'êtes pas lié à une vue

> getFishes
< list (vide)  # if there is no fishes in the view

> getFishes
< list [PoissonRouge at 90x4,10x4,5] [PoissonClown at 20x80,12x6,5]  #for example

> getFishesContinously
< les poissons seront envoyés chaque 5 secondes
      #the client should be ready to get the fishes every 5 secondes


### ping protocol

> ping
< pong  #if the timeout is not reached

> ping
< bye   #if the timeout is reached (the client is deconnected and can't send any messages after that)


### log out protocol

> log out
< bye  #the client is deconnected and can't send any messages after that


 



