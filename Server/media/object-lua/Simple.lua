PrintUpdate = true;

function Create()
  print( "Lua object script created." );
end

function Destroy()
  print( "Lua object script destroyed." );
end

function Update()
  if PrintUpdate == true then
    print( "Lua object script updated." );
    PrintUpdate = false;
  end
end