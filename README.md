# WebCache
A simple way to cache a basic JSON api.

The purpose of this lib is to give an auto refreshed aggregation of a json api (GET only).

Conceptual representation:
````
GET webservice/myobjectA
{
    "potato" : true
}
GET webservice/myobjectB
{
    "chips" : false
}

would give you :
{
    "webservice" : {
        "myobjectA" : { "potato" : true},
        "myobjectB" : { "chips" : false}
    }
}
````


Some restrictions are applicable:
- only GET is supported
- authentication isn't supported
- the lib will merge the retrieved json => you can't have overlaps between urls
ex :
````
GET webservice/myobjectA
{
    "potato" : true
}
GET webservice/myobjectA/potato
{
    "chips" : false
}

Would give and undefined behavior
````


#How to build
You'll need a working libcurl / googletest installation and don't forget the submodules checkout !
````
mkdir build
cd build
cmake ..
make -j $(nproc)
````

#how to use
Check the test folder for samples
