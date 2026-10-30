2 test levels. In: /Game/Levels/Flight and /Game/TrainStation/Maps/Demonstration

Hit play in Flight level and move around and the agetns will automatically follow you.
Hit play in the trainstation level and the agents will go to 1 target.

To use the plugin:
Place a BoundingVolume in a level.
Give it a size.
Created a pawn or actor, give it a DFNNavAgent component.
Call MoveTowards, with Target = DFNNavAgent component, a Destination or a Targetactor.
If a Targetactor is assigned then Destination is ignored.
Give it a acceptance radius.
And done.