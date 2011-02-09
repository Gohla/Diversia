Timer = 0;
Exploded = false;

function Frame( TimeSinceLastFrame )
  Timer = Timer + TimeSinceLastFrame;
  
  if Timer > 3.0 and not Exploded then
    -- Explode, enable particles and play sound.
    if Object:HasComponent( "Smoke" ) then Object:GetComponent( "Smoke" ).Enabled = true; end
    if Object:HasComponent( "Fire" ) then Object:GetComponent( "Fire" ).Enabled = true; end
    if Object:HasComponent( "Explosion" ) then Object:GetComponent( "Explosion" ):Play(); end
    Exploded = true;
  end
  
  if Timer > 5.0 then
    -- Destroy object, bomb has exploded
    Object:Destroy();
  end
end