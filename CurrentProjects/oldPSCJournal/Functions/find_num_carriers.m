% Created by Dakota Flanary
function [num_carrier_list,good_carriers] = find_num_carriers(data,snr,threshold)
%find_num_carriers Finds the number of carriers above the specified
%                   threshold
%   Compares each carrier with the noise on the carrier and if the
%   difference is greater than the threshold it is counted as a good
%   carrier.  If there is no data a nan is inputed for the number of
%   carriers.  Alternate method of finding the average noise floor is done
%   by uncommenting the commented code and commenting out the current for
%   loop where the variable is loc.
[num_carriers,num_runs] = size(data);
num_carrier_list = zeros(num_runs,1);
good_carriers = zeros(num_carriers,num_runs);
for run = 1:num_runs
    num_good = 0;
    for carrier = 1:num_carriers
        if data(carrier,run) * snr >= threshold
            good_carriers(carrier,run) = 1;
            num_good = num_good + 1;
        else
            good_carriers(carrier,run) = 0;
        end
    end
    num_carrier_list(run) = num_good;
end
end

