function [secrecyCapacityPerLocation, secrecyCapacityPerCarrier] = calculateSecrecyCapacity(bobsData, evesData)
%calculateSecrecyCapacity Determines the difference in bob and eves capacity

%   Compares each of Bob's carriers to Eve's carriers. For carriers where
%   Bob's capacity is greater than Eve's, the secrecy capacity is the
%   difference. When Eve's capacity is greater than Bob's, the secrecy
%   capacity is simply 0.
%   Takes in data in row x carrier format. 

    [numRows, numCarriers] = size(bobsData);
    
    secrecyCapacityPerCarrier = zeros(numRows,numCarriers);
    secrecyCapacityPerLocation = zeros(numRows);
    
    for bobsLocation = 1:numRows
        for evesLocation = 1:numRows
            temp = 0;
            for carrier = 1:numCarriers
                if (bobsData(bobsLocation, carrier) <= evesData(evesLocation, carrier))
                    secrecyCapacityPerCarrier(bobsLocation, carrier) = 0;
                else
                    secrecyCapacityPerCarrier(bobsLocation, carrier) = ...
                        bobsData(bobsLocation, carrier) - evesData(evesLocation, carrier);
                    temp = temp + secrecyCapacityPerCarrier(bobsLocation, carrier);
                end
            end
            secrecyCapacityPerLocation(bobsLocation, evesLocation) = temp;
        end
    end

end

