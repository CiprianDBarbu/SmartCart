# Smart Cart

Following the SARS-CoV-2 virus pandemic (COVID19), a place where the whole population is constantly moving is the supermarket. A problem encountered by most customers in supermarkets is the unhygienic use the shopping cart, some people being forced, in some stores, to use one. Due to the fact that the constant hygiene of the handle (especially) is difficult to achieve because the cart itself is constantly used by a very large number of people we determined that the object itself is a permanent source of bacteria and viruses.

In order to solve the identified problem, we have developed an innovative prototype that allows a cart to be used hands-free. The new concept ensures customers a safe experience from a hygienic point of view, it works based on the following principle: In the component structure of the stroller will be included a motion sensor that will help to move it automatically in the direction of travel of the user keeping a certain predetermined distance from it.

In addition, a device will be added to record the price of each product chosen by the buyer, summing them up at the end to determine the monetary value of the basket.

## Getting Started

Git clone this project in your machine.

### Prerequisites

You will need to have a C++ compiler. I used g++ that came preinstalled. Check using `g++ -v`

You will need to install the [Pistache](https://github.com/pistacheio/pistache) library.

### Building

g++ cart_example.cpp -o cart -lpistache -lcrypto -lssl -lpthread

### Running

./cart

## Built With

* [Pistache](https://github.com/pistacheio/pistache) - Web server
