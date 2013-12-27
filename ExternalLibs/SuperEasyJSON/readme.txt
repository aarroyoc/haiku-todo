                        SuperEasyJSON
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

PURPOSE:
========

Why write another C++ JSON library? I was getting frustrated
with the other offerings for a variety of reasons. 

1. I didn't want to install Boost or any other library 
2. I didn't want to install any programs/scripts that I didn't already have just to
    create the library (I'm looking at you flex and bison and python)
3. I didn't want to have to run 'make'
4. I just wanted a header and cpp file that I could drop into
    any project and it would work. 
5. I wanted an interface that was easy to use

This library is easy to use, requires no installation (other than adding
it into your project), and doesn't depend on anything else.

It is NOT the library for you if you demand extreme speed or amazing
memory performance. There are already other libraries out there if that's
what you need. For my purposes, I'm neither creating gigantic JSON objects
nor am I making lots of them nor am I making them in some super critical
time-dependent block of code that any of this would have any effect
whatsoever. As an iOS game programmer, I don't think I'd ever be in this
position. 

Could it have been written more efficiently? Of course. But for my
needs, the performance cost of the code as-is is negligible, and the
trade-off was I was able to write and test it very quickly. If
I needed a high performance implementation I'd have used an existing
library or written things completely differently.

So if you want a brain-dead simple library to get started with, and
you can live with the performance trade-offs, then give it a shot.

Any bugs, please email me at my gmail account: jeff.weinstein.
Please use the subject-line "EasyJSON" followed by whatever else. 
Preferably not profanity.

INSTALLATION:
============
Drop json.h and json.cpp into your project. Seriously, that's it.

USAGE:
======
Just include "json.h" and you're ready to go. See the examples below
for more help. 

You need to make a json::Object variable, and then fill it with
whatever you like. Populate it like you would a std::map. For
example: my_obj_instance["some key"] = <some value>

Where <some value> is an int/flot/bool/string/array/object.

What about arrays? Make a json::Array instance, and fill
it with whatever values you like using its .push_back method.
It acts like a std::vector. You can fill it with a mix of types.

You can access any element in a json::Array with the familiar [] notation.
The same applies for a json::Object, supplying the string key to get
the value. 

Everything is stored as a json::Value, which has a lot of convenience
operator methods to cast it (or construct it) to (or from) any of the
valid types.

For the NULL type, simply use an empty json::Value instance.

You can use the new C++11 style for loop with Array and Object like so:

for (auto& kv : my_object)...



EXAMPLE:
========

A less complicated example:
	#include "json.h"
	
	json::Object obj;
	obj["int"] = 1;
	obj["float"] = 1.1f;
	obj["bool"] = true;
	obj["string"] = "hi";
	
	std::string str = json::Serialize(obj);
	
The value of "str" winds up being this: 
{"bool":true,"float":1.100000,"int":1,"string":"hi"}

Which looks a lot nicer when it's run through a formatter:
{
   "bool":true,
   "float":1.100000,
   "int":1,
   "string":"hi"
}

Now for a complicated example with lots of nesting:
	#include "json.h"
	
	json::Object sub_obj;
	sub_obj["string"] = "sub_obj's string";

	json::Array sub_array;
	sub_array.push_back("sub_array string 1");
	sub_array.push_back("sub_array string 2");
	sub_array.push_back(1.234f);

	json::Array a;
	a.push_back(10);
	a.push_back(10.0f);
	a.push_back(false);
	a.push_back("array a's string");
	a.push_back(sub_obj);
	a.push_back(sub_array);

	json::Object sub_obj3;
	sub_obj3["sub_obj3 string"] = "whoah, I'm sub_obj3's string";
	sub_obj3["sub_obj3 int"] = 31337;

	json::Object sub_obj2;
	sub_obj2["int"] = 1000;
	sub_obj2["bool"] = false;
	sub_obj2["x"] = sub_array;
	sub_obj2["z"] = sub_obj3;

	json::Object obj;
	obj["int"] = 1;
	obj["float"] = 1.1f;
	obj["bool"] = true;
	obj["string"] = "come here often?";
	obj["array"] = a;
	obj["sub_obj2"] = sub_obj2;
	obj["null value"] = json::Value();

	std::string str = json::Serialize(obj);

The value of "str" winds up being this (returns added for wrapping purposes)

{"array":[10,10.000000,false,"array a's string",{"string":"sub_obj's string"},[
"sub_array string 1","sub_array string 2",1.234000]],"bool":true,
"float":1.100000,"int":1,"null value":null,"string":"come here often?",
"sub_obj2":{"bool":false,"int":1000,
"x":["sub_array string 1","sub_array string 2",1.234000],
"z":{"sub_obj3 int":31337,"sub_obj3 string":"whoah, I'm sub_obj3's string"}}}

Which looks a whole lot easier to understand formatted thusly:
{
   "array":[
      10,
      10.000000,
      false,
      "array a's string",
      {
         "string":"sub_obj's string"
      },
      [
         "sub_array string 1",
         "sub_array string 2",
         1.234000
      ]
   ],
   "bool":true,
   "float":1.100000,
   "int":1,
   "null value":null,
   "string":"come here often?",
   "sub_obj2":{
      "bool":false,
      "int":1000,
      "x":[
         "sub_array string 1",
         "sub_array string 2",
         1.234000
      ],
      "z":{
         "sub_obj3 int":31337,
         "sub_obj3 string":"whoah, I'm sub_obj3's string"
      }
   }
}