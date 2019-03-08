% Created by Dakota Flanary
function [carrier_list] = find_num_carriers(data,snr,threshold)
%find_num_carriers Finds the number of carriers above the specified
%                   threshold
%   Compares each carrier with the noise on the carrier and if the
%   difference is greater than the threshold it is counted as a good
%   carrier.  If there is no data a nan is inputed for the number of
%   carriers.  Alternate method of finding the average noise floor is done
%   by uncommenting the commented code and commenting out the current for
%   loop where the variable is loc.
[num_locations,used_carriers] = size(data);
num_carrier_list = zeros(num_locations,1);
for location = 1:num_locations
    num_carriers = 0;
    if isnan(data(location,1))
        num_carriers = nan;
    else
        for carrier = 1:used_carriers
            if data(location,carrier) * snr >= threshold
                num_carriers = num_carriers + 1;
            end
        end
    end
    num_carrier_list(location) = num_carriers;
end
carrier_list = num_carrier_list;
end