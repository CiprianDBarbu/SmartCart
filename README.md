# Example IoT Device
 
## Getting Started
 
Git clone this project in your machine.
 
### Prerequisites
 
Build tested on Ubuntu Server. Pistache doesn't support Windows, but you can use something like [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) or a virtual machine with Linux.
 
You will need to have a C++ compiler. I used g++ that came preinstalled. Check using `g++ -v`
 
You will need to install the [Pistache](https://g...content-available-to-author-only...b.com/pistacheio/pistache) library.
On Ubuntu, you can install a pre-built binary as described [here](http://p...content-available-to-author-only...e.io/docs/#installing-pistache).
 
### Building
 
#### Using Make
 
### Manually
 
A step by step series of examples that tell you how to get a development env running
 
You should open the terminal, navigate into the root folder of this repository, and run\
`g++ -std=c++1z cart_main.cpp -o cart -lpistache -lcrypto -lssl -lpthread`
 
This will compile the project using g++, into an executable called `cart` using the libraries `pistache`, `crypto`, `ssl`, `pthread`. You only really want pistache, but the last three are dependencies of the former.
Note that in this compilation process, the order of the libraries is important.
 
### Running
 
To start the server run\
`./cart`
 
Your server should display the number of cores being used and no errors.
 
To test, open up another terminal, and type\
`curl http://localhost:9080/ready`
 
Number 1 should display.
 
Now you have the server running
 
Exemplu pt alte operații
3. setăm banii curl -X POST http://localhost:9080/settings/money/80 - Input Buffer
4. verificam banii curl -X GET http://localhost:9080/settings/money - Output Bufer
5. adaug in wishlist sa zicem 2-3 produse
-al doilea Input Bufer
curl -X POST -H "Content-Type: application/json" -d '{"name":"produs1", "value":27, "rating":28, "x":3, "y":3, "z":3}' localhost:9080/wishlist
curl -X POST -H "Content-Type: application/json" -d '{"name":"produs2", "value":70, "rating":503, "x":7, "y":3, "z":3}' localhost:9080/wishlist
curl -X POST -H "Content-Type: application/json" -d '{"name":"produs3", "value":1, "rating":28, "x":3, "y":3, "z":3}' localhost:9080/wishlist
6. verificam, cate putem lua maxim de banii astia
 
curl -X GET http://localhost:9080/maxCumparate
7. afisam de drumul minim care trece prin toate produsele din wishlist .
curl -X GET http://localhost:9080/drumMinim
 
8. afisam cati bani avem ( ar trebui money = 80, pt ca nu am cumparat nimic) .
curl -X GET http://localhost:9080/buget
9. cumparam un produs !
curl -X POST -H "Content-Type: application/json" -d '{"name":"produs1", "value":27, "rating":28, "x":3, "y":3, "z":3}' localhost:9080/products
 
10. ar trebui sa avem 80 - 27 money
curl -X GET http://localhost:9080/buget
 
11. putem vedea de exemplu ce am cumparat ->
al doilea ouptut Buffer
curl -X GET http://localhost:9080/products
 
12. verificam dacă am lut tot wishlistul
curl -X GET http://localhost:9080/verifWL
 
 
## Built With
 
* [Pistache](https://g...content-available-to-author-only...b.com/pistacheio/pistache) - Web server
 
## License
 
This project is licensed under the Apache 2.0 Open Source Licence - see the [LICENSE](LICENSE) file for details
