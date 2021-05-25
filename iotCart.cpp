
#include <algorithm>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>
#include <nlohmann/json.hpp>
#include <signal.h>

using namespace std;
using namespace Pistache;
using json = nlohmann::json;
void addJsonContentTypeHeader(Http::ResponseWriter &response) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
}

// This is just a helper function to preety-print the Cookies that one of the enpoints shall receive.
void printCookies(const Http::Request& req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto& c: cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

// Some generic namespace, with a simple function we could use to test the creation of the endpoints.
namespace Generic {

    void handleReady(const Rest::Request&, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

}

// Definition of the CartEnpoint class 
class CartEndpoint {
public:
           // General advice: pay atetntion to the namespaces that you use in various contexts. Could prevent headaches.

    float getPred(float rating, int were_given)
    {
        // maximum number of stars to distribute
    
        return rating * were_given * 1/2;
    
    }
    struct Money
    {
        std:: string name;
        float value;
    };
    struct Position
    {
        float x, y, z;
        Position operator-(const Position &that)
        {
            return Position{x - that.x, y - that.y, z - that.y};
        }
        float norma()
        {
            return x*x+y*y+z*z;
        }
    
        bool operator==(const Position &that)const
        {
            return x==that.x&&y==that.y&&z==that.z;
        }
    
    };
    struct Product
    {
        std:: string name;
        float value;
        float rating;
        Position pos;
        bool operator== (const Product& that)
        {
            return name==that.name&&that.value==value&&that.rating==rating&&pos==that.pos;
        }
        Product () {};
        Product(string name, float value, float rating, float x, float y, float z) : name(name), value(value), rating(rating), pos{x, y, z} {};
       static Product parse(const vector<string> &v) {
        if (v.size() != 6) {
            throw "Invalid Product, cannot parse";
        }

        return Product(v[0], stof(v[1]), stof(v[2]), stof(v[3]), stof(v[4]), stof(v[5]));
    }
    };
    void addJsonContentTypeHeader(Http::ResponseWriter &response) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
}
void to_json(nlohmann::json& j, const Product& p) {
    j = {
            {"name", p.name},
            {"value", p.value},
            {"rating", p.rating},
            {"x", p.pos.x},
            {"y", p.pos.y},
            {"z", p.pos.z}
    };
}

    void from_json(const nlohmann::json& j, Product& p) {
    j.at("name").get_to(p.name);
    j.at("value").get_to(p.value);
    j.at("rating").get_to(p.rating);
    Position now;
    j.at("x").get_to(p.pos.x);
    j.at("y").get_to(p.pos.y);
    j.at("z").get_to(p.pos.z);
}

void insertProduct(const Rest::Request &request, Http::ResponseWriter response) {
    // curl -X POST -H "Content-Type: application/json" -d '{"date":"2021-03-23","id":1,"max_temp":20,"name":"test","quantity":3}' localhost:9080/products
    addJsonContentTypeHeader(response);

    try {
        auto bodyJson = json::parse(request.body());
        Product p;
        from_json(bodyJson, p);
        response.send(Http::Code::Ok, "OK");
        cout << "S-a adaugat ! \n";
        mwv.add_cumparate(p);
    } catch (...) {
        response.send(Http::Code::Bad_Request, "EROARE");
        return;
    }

    response.send(Http::Code::Ok);
}
void insertWishlist(const Rest::Request &request, Http::ResponseWriter response) {
    // curl -X POST -H "Content-Type: application/json" -d '{"date":"2021-03-23","id":1,"max_temp":20,"name":"test","quantity":3}' localhost:9080/products
    addJsonContentTypeHeader(response);

    try {
        auto bodyJson = json::parse(request.body());
        Product p;
        from_json(bodyJson, p);
        response.send(Http::Code::Ok, "S-a adăugat în wishlist!");
        mwv.add_wishlist(p);
    } catch (...) {
        response.send(Http::Code::Bad_Request, "EROARE");
        return;
    }

    response.send(Http::Code::Ok);
}

void getAllProducts(const Rest::Request &request, Http::ResponseWriter response) {
    addJsonContentTypeHeader(response);

    json aux;
    json j;
    int cnt = 0;
    vector<Product>L = mwv.vecCumparate();
    for(auto it : L) {
       to_json(aux, it);
       j[to_string(cnt)] = aux;
       cnt += 1;
    }
    response.send(Http::Code::Ok, j.dump());
}

    explicit CartEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    // Initialization of the server. Additional options can be provided here
    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(static_cast<int>(thr));
        httpEndpoint->init(opts);
        // Server routes are loaded up
        setupRoutes();
    }

    // Server is started threaded.  
    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();
    }

    // When signaled server shuts down
    void stop(){
        httpEndpoint->shutdown();
    }

private:
    void setupRoutes() {
        using namespace Rest;
        // Defining various endpoints
        // Generally say that when http://localhost:9080/ready is called, the handleReady function should be called. 
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/auth", Routes::bind(&CartEndpoint::doAuth, this));
        Routes::Post(router, "/settings/:settingName/:value", Routes::bind(&CartEndpoint::setSetting, this));
        Routes::Get(router, "/settings/:settingName/", Routes::bind(&CartEndpoint::getSetting, this));
        Routes::Get(router, "/products", Routes::bind(&CartEndpoint::getAllProducts, this));
        Routes::Post(router, "/products", Routes::bind(&CartEndpoint::insertProduct, this));
        Routes::Post(router, "/wishlist", Routes::bind(&CartEndpoint::insertWishlist, this));
        Routes::Get(router, "/buget", Routes::bind(&CartEndpoint::getBugetRamas, this));
        Routes::Get(router, "/drumMinim", Routes::bind(&CartEndpoint::getDrumMinim, this));
        Routes::Get(router, "/maxCumparate", Routes::bind(&CartEndpoint::getMaxCumparate, this));
        Routes::Get(router, "/verifWL", Routes::bind(&CartEndpoint::getLuatTot, this));
        Routes::Get(router, "/knapsack", Routes::bind(&CartEndpoint::getKnapsack, this));
    }

  
    void doAuth(const Rest::Request& request, Http::ResponseWriter response) {
        // Function that prints cookies
        printCookies(request);
        // In the response object, it adds a cookie regarding the communications language.
        response.cookies()
            .add(Http::Cookie("lang", "en-US"));
        // Send the response
        response.send(Http::Code::Ok);
    }

    // Endpoint to configure one of the Cart's settings.
    void setSetting(const Rest::Request& request, Http::ResponseWriter response){
        // You don't know what the parameter content that you receive is, but you should
        // try to cast it to some data structure. Here, I cast the settingName to string.
        auto settingName = request.param(":settingName").as<std::string>();
        // This is a guard that prevents editing the same value by two concurent threads. 
        Guard guard(CartLock);

        
        string val = "";
        if (request.hasParam(":value")) {
            auto value = request.param(":value");
            val = value.as<string>();
        }

        // Setting the Cart's setting to value
        int setResponse = mwv.set(settingName, val);
        

        // Sending some confirmation or error response.
        if (setResponse == 1) {
            response.send(Http::Code::Ok, settingName + " was set to " + val);
        }
        else {
            response.send(Http::Code::Not_Found, settingName + " was not found and or '" + val + "' was not a valid value ");
        }

    }

    void getMaxCumparate(const Rest::Request& request, Http::ResponseWriter response){

        Guard guard(CartLock);


        response.send(Http::Code::Ok, "Din lista de wishlist poți cumpăra maxim" + to_string(mwv.maxRamase()) + "produse!\n");
    }

    // Setting to get the settings value of one of the configurations of the Cart
    void getSetting(const Rest::Request& request, Http::ResponseWriter response){
        auto settingName = request.param(":settingName").as<std::string>();

        Guard guard(CartLock);

        string valueSetting = mwv.get(settingName);

        if (valueSetting != "") {

            // In this response I also add a couple of headers, describing the server that sent this response, and the way the content is formatted.
            using namespace Http;
            response.headers()
                        .add<Header::Server>("pistache/0.1")
                        .add<Header::ContentType>(MIME(Text, Plain));

            response.send(Http::Code::Ok, settingName + " is " + valueSetting);
        }
        else {
            response.send(Http::Code::Not_Found, settingName + " was not found");
        }
    }

     void getLuatTot(const Rest::Request& request, Http::ResponseWriter response) {
         Guard guard(CartLock);
         if(!mwv.aLuatTot())
         response.send(Http::Code::Ok, "Nu ai cumpărat tot wishlistul încă!\n");
         else response.send(Http::Code::Ok, "Ai cumpărat tot wishlistul!\n");
     }
     void getBugetRamas(const Rest::Request& request, Http::ResponseWriter response){

        Guard guard(CartLock);


        response.send(Http::Code::Ok, "Mai ai : " + to_string(mwv.calculeazabugetramas()) + "bani");
    }
    void getKnapsack(const Rest::Request& request, Http::ResponseWriter response){

        Guard guard(CartLock);


        response.send(Http::Code::Ok, "De bani care ii ai poți avea suma ratingurilor cumpărată de: " + to_string(mwv.maxRating()));
    }
    void getDrumMinim(const Rest::Request& request, Http::ResponseWriter response){

        Guard guard(CartLock);
        string s;

        response.send(Http::Code::Ok, "Drumul minim în care parcurgi tot wishlistul are lungimea: " + to_string(mwv.drumMinim()));
    }
    // Defining the class of the Cart. It should model the entire configuration of the Cart
    class Cart {
    public:
        explicit Cart(){
            
         }

        // Setting the value for one of the settings. Hardcoded for the defrosting option
        int set(std::string name, std::string value){
            if(name == "money"){
                money.name = name;
                if(numar(value)){
                    money.value = std::stof(value);
                    return 1;
                }
            }
            return 0;
        }

        // Getter
        string get(std::string name){
            if(name == "money")
            {
                return std::to_string(money.value);
            }
            else{
                return "";
            }
        }
        float calculeazabugetramas()
        {
            // care este bugetul ramas ?
            float res = 0;
            for(auto i : lista_cumparate)
                res += i.value;
            return money.value - res;
        }
    
        Position direction(Position posClient)
        {
            return posClient - posCart;
        }

        float maxRating() {
            float am = calculeazabugetramas();
            int M = 1 << lista_necesara.size();
            float best = 0;
            for(int i = 0; i < M; i++) {
                float sumPrice = 0, sumRating = 0;
                for(int j = 0; j < lista_necesara.size(); j++) {
                   if(i & (1 << j)) {
                      sumPrice += lista_necesara[j].value;
                      sumRating += lista_necesara[j].rating;
                   }
                }
                if(sumPrice <= am && sumRating > best)
                  best = sumRating;
            }
            return best;
        }
    
        // Funcționalitatea 2: care este numpărul maxim de produse pe care îl putem cumpăra din cele rămase, astfel încât să nu depășim suma de bani?
        int maxRamase()
        {

            // cate produse mai intra in cos ?
            auto aux = lista_necesara;
            sort(aux.begin(), aux.end(), [&] (const Product &p1, const Product &p2)
            {
                return p1.value < p2.value;
            });
    
            float mai_am = calculeazabugetramas();
            int cate = 0;
            for(int i = 0; i < aux.size(); i++)
            {
                if(mai_am - aux[i].value >= 0)
                {
                    mai_am -= aux[i].value;
                    cate++;
                }
            }
            return cate;
        }
    
        // Funcționalitatea 3: care este drumul minim pe care sa-l parcurgem, astfel încât să cumpărăm toată lista de produse rămase?
        // unde prin drum minim ne referim la suma distanțelor între două produse cumpărate consecutiv.
        float drumMinim()
        {
            std::vector<int>p(lista_necesara.size());
            for(int i = 0; i < p.size(); i++)
                p[i] = i;
            float ans = 1e15;
            do
            {
                float cost = 0;
                for(int i = 0; i < (int)p.size() - 1 && cost < ans; ++i)
                {
                    Position p1 = lista_necesara[i].pos;
                    Position p2 = lista_necesara[i + 1].pos;
                    cost += (p1 - p2).norma();
                }
                 if(cost < ans)
                 {
                        ans = cost;
                 }
            }
            while(next_permutation(p.begin(), p.end()));
            if(lista_necesara.size() == 0) ans = 0;
            return ans;
        }
    
        // Funcționalitatea 4: a cumparat toate produsele necesare? (el putea sa cumpere și unele care nu sunt în wishlist!)
        bool aLuatTot()
        {
            for(int i = 0; i < (int)lista_necesara.size(); i += 1)
            {
                bool Found = 0;
                for(int j = 0; j < (int)lista_cumparate.size(); j += 1)
                  if(lista_cumparate[j] == lista_necesara[j])
                    Found = 1;
                if(!Found) return 0;
            }
            return 1;
        }
        //verific daca ie numar
        bool numar(string s) {
        int e = 0;
        for(auto c : s) {
            if(c == '.') e++;
            if(!isdigit(c) && c != '.') return 0;
        }
        return (e <=1);
}
vector<Product>vecCumparate() {
    return lista_cumparate;
}
void add_cumparate(Product p) {
    lista_cumparate.push_back(p);
}
void add_wishlist(Product p) {
    lista_necesara.push_back(p);
}

    private:
        // Defining and instantiating settings.
            // Defining and instantiating settings.
        Money money;
        Position posCart;

    
    
        std::vector<Product>lista_necesara;
        std::vector<Product>lista_cumparate;
    };

    // Create the lock which prevents concurrent editing of the same variable
    using Lock = std::mutex;
    using Guard = std::lock_guard<Lock>;
    Lock CartLock;

    // Instance of the Cart model
    Cart mwv;

    // Defining the httpEndpoint and a router.
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main(int argc, char *argv[]) {

    // This code is needed for gracefull shutdown of the server when no longer needed.
    sigset_t signals;
    if (sigemptyset(&signals) != 0
            || sigaddset(&signals, SIGTERM) != 0
            || sigaddset(&signals, SIGINT) != 0
            || sigaddset(&signals, SIGHUP) != 0
            || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0) {
        perror("install signal handler failed");
        return 1;
    }

    // Set a port on which your server to communicate
    Port port(9080);

    // Number of threads used by the server
    int thr = 2;

    if (argc >= 2) {
        port = static_cast<uint16_t>(std::stol(argv[1]));

        if (argc == 3)
            thr = std::stoi(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;

    // Instance of the class that defines what the server can do.
    CartEndpoint stats(addr);

    // Initialize and start the server
    stats.init(thr);
    stats.start();


    // Code that waits for the shutdown sinal for the server
    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0)
    {
        std::cout << "received signal " << signal << std::endl;
    }
    else
    {
        std::cerr << "sigwait returns " << status << std::endl;
    }

    stats.stop();
}