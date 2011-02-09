function AreaChange( AreaObject, Entered ) 
  if Entered then 
    print( "Object " .. AreaObject.DisplayName .. " entered area." );
    if AreaObject:HasComponent( "RigidBody" ) then
      AreaObject:GetComponent( "RigidBody" ):ApplyCentralImpulse( Vector3.new( 0, 10, 0 ) );
    end
  else 
    print( "Object " .. AreaObject.DisplayName .. " left area." ); 
  end 
end