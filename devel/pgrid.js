// ##############################
// #
// # Let's pseudo-code a grid-search pattern
// # Basically, move in a straight vecotr (+-1,0 or 0,+-1) until encounter [something]
// # If we still are executing, means that [something] wasn't our goal, so decide
// # on new vector.
// # If vector was (1,0),  change vector to (0,1) for one step, then (-1,0)
// # If vector was (0,1),  change vector to (1,0) for one step, then (0,-1)
// # If vector was (-1,0), change vector to (...) ...
// # ...
// #
// # For this proc, we need:
// # - Last known vector 
// # - Sensor Input (e.g. circle of objects around, ANSITextSensor)
// # 	- for use with "can we move in dir of vector?"
// #
// # Output: SimpleXYMovementVector Node
// #
// # Store in context: Last Known Vector
// #
// ##############################


// proc	=> $proc,
// context	=> $context,
// agent	=> $agent,
// input_nodes	=> $input_nodes,

var ctxNode = context.node();

// SimpleXYMovementVector stores (x,y) as attributes of the data() function:
// var x = lastKnownVector.data().get("x");
var lastKnownVector = ctxNode.linked_nodes("SimpleXYMovementVector",1); // 1 = return first
//var  = searchResults.length ? searchResults[0] : null; 
//print("Debug: lastKnownVector: "+lastKnownVector.length+", valid?"+(lastKnownVector?true:false));

var v = 
{ 
	x: lastKnownVector ? lastKnownVector.data().get("x") : 0,
	y: lastKnownVector ? lastKnownVector.data().get("y") : 0
}; 

// SimpleTextVisualContext store visual context as data hanging off it in eight different
// attributes named, appropriatly: 
// N,S,E,W,NE,SE,NW,SW
// or
// 0-1 01 01 0-1 1-1 11 -1-1 -11
// Both keys are valid.
// The data stored in those atributes is a hash of {char: "character", attr:[,,,,]} where attr is a list of flags/values for ansi attributes
// visualContext should be current as of the LAST move
var visualContext = ctxNode.linked_nodes("SimpleTextVisualContext",1); // 1 forces to return first node not a list
//var  = searchResults.length ? searchResults[0] : null; 

var attrToCheck = ""+(v.x)+""+(v.y);

print("Debug: attrToCheck: ", attrToCheck);
print("Visual Context: ",visualContext);

var visualInfo = visualContext.data().get(attrToCheck);
if(visualInfo)
	print("Visual Info: ", visualInfo.char, ", attr: ", visualInfo.attr);
else
	print("No visual info at "+attrToCheck);


print("Changing last known vector to 01");
lastKnownVector.data().set("x",0);
lastKnownVector.data().set("y",1);
//lastKnownVector.data().update();


return lastKnownVector;