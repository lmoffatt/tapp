# tapp
Type as Proposition Programming. 


The Curry-Howard correspondance allows to convert mathematical proof to a computer programs. In this experimental library we are interested on the converse, that is to think of computer programs as reasonings on propositions. One of the ideas of this correspondance is that Types are equivalent to propositions and variables are sentinels of the truthness of a given proposition.
The idea is the converse, to think of each Type as hidden a proposition on it, and the beauty of this approach consists in extend the regular definition of Types to functions, so the Type of a function is set by the proposition that it complies: its contract (the postconditions). In this framework we think of all the functions that comply with a given post-condition on their output as belonging to the same type. 

So, in other words we lump toghether three fundamental concepts : Types, contracts and Tests. 
The fundamental idea is that that a variable exists is a proof that it complies the contract specified for its type and tests are performed to verify that this is indeed the case. 
We use the idea that a false proposition is a type without a valid constructor, so if we are able to build a variable, that proofs that it fullfills its contract. The idea is that types summarize what we already know. In a way we can think of types as ideas. 

Also, this idea of types as propositions allow for a more natural definition of clasess with methods. Sometimes the contracts are better defined recursively using several element-types and function-types in a closed loop of conditions. Mathematical objects like groups, rings or  linear transformation are defined in this way. So, we can think of object-types as a tuple of several interlinked element-types and function-types. So, objects would not be arbitrary anymore, they should have a series of interdependent contracts on their parts to have a right to exist. 

Type as Proposition Programming is a powerfull idea for its onthological force: we sort of see what variables, element-types, function-types and object-types are. Can it also have a practical use? 
Two things comes to mind. 
1. A way to define what a type is really about: a way to write enforzable contracts. That is very important for code comprehension. 
2. A way to automatically define tests, right in the definition of the clases. 

Another advantage of TaPP is that Types should be independent pieces of code that can be used and tested independently. 

So, TaPP should be able to mix with regular code without problem. We should be able to start TaPPize the code progressively, that is a good requirement, TaPPize what is necessary. 

So, lets fix our objectives on those two achievable ones. 

1. Provide several mechanisms for definining the contracts that define a given Type. 
2. Provide several mechanisms for building tests to assertain the compliance of those contracts. 


Lets think of a first case: the subObject. 

SubObject

the idea of a subobject is a subset of a Type that complies with a given proposition, that is that its defining predicate applied to the object representation of the subobject always return true. In principle it would be thought as a subclass, but it might also cover the definition of functions. 


The suboject can be thought of an embedding in the Object, so there should bean inyection function that returns an Object taking a subObject as input. 
Conversely, there should be an predicate function that indicates for variables of type Object if they have a SubObject representation (that is congruent to the defining predicate function of the subObject) and a third function that returns a Maybe<SubObject> type taking an Object as input and a forth sg that takes a Maybe<SubObject> and returns a  Maybe<Object> 
  
  
One possibility is to provide also an unsafe function that takes an Object (which is warrantied to fullfil the predicate) and returns a SubObject with undefined behavior in case that an Object which predicates false is given. 
  
So we have and unsafe function g Object->SubObject,  a predicate p Object->boolean that warranties that a SubObject can be safely constructed and an always safe function f SubObject->Object. 

For all s in SubObject
g(f(s)) == s
  
for all o in Object where p(0)
f(g(o))==o

So, the idea is forbid the constructions of SubObjects from Objects that predicate false. One way to do that is to use the template class Maybe_error in the following way
  
   safe_p: 
  Object->Maybe<Object> conditional on p
  
  safe_g
  g(Mabybe<Object>) -> Maybe<SubObject>
  
  safe_f
  f(Maybe<SubOjbect>).> Maybe<Object>
 
  
Furthermore, we can bu  
  



















