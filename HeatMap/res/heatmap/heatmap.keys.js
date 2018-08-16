var shiftKey, altKey, CtrlKey;

d3.select("body")
  .on("keydown", function(){ keyDown(d3.event.keyCode); } );
  
function keyDown( key ) {
 
  if( key == 37 ) // left arrow
  {
    moveSelection("left");
  }
  else if( key == 38 ) // up arrow
  {
    moveSelection("last");
  }
  else if( key == 39 ) // right arrow
  {
    moveSelection("right");
  }
  else if( key == 40 ) // down arrow
  {
    moveSelection("first");
  }
}