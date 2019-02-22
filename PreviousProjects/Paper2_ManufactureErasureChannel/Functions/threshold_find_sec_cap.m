% Created by Dakota Flanary
function [sec_cap] = threshold_find_sec_cap(bob,eve)
%threshold_find_sec_cap Finds the number of carriers that are good for Harrison and the
%others also finds the number of carriers that are only good for Harrison
%   Detailed explanation goes here
sec_cap = 0;
for carrier = 1:45
    match = false;
    if bob(1,carrier) ~= 0
        if eve(1,carrier) ~= 0
            match = true;
        end
    else
        match = true;
    end
    if match == false
        sec_cap = sec_cap + 1;
    end
end
end

