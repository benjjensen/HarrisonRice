% Runs through a range of decibels and determines how many carriers only
    % Harrison has at each decibel level, and then stores and saves an
    % array

close all;

load('tx2_pwelch.mat');
load('tx2camacho_pwelch.mat');
load('tx2chambers_pwelch.mat');
load('tx2conference_pwelch.mat');
load('tx2harrison_pwelch.mat');
load('tx2smalley_pwelch.mat');

min = 25;
max = 32;
stepSize =.1;
counter = 1;

harrisonOnlyCarriers = zeros((max-min)/stepSize, 2);
for dblimit = min:stepSize:max
    
    [har_specific_carriers_above, ~] = specCarriers(tx2harrison_pwelch, 1, dblimit);
    [sma_specific_carriers_above, ~] = specCarriers(tx2smalley_pwelch, 1, dblimit);
    [cha_specific_carriers_above, ~] = specCarriers(tx2chambers_pwelch, 1, dblimit);
    [cam_specific_carriers_above, ~] = specCarriers(tx2camacho_pwelch, 1, dblimit);
    [con_specific_carriers_above, ~] = specCarriers(tx2conference_pwelch, 1, dblimit);
    
    onlyHarrisons = har_specific_carriers_above;
    ocount = 0;
    for car = 1:64
        if ((har_specific_carriers_above(car, 2) ~= 0) && (sma_specific_carriers_above(car,2) ~= 0))
            onlyHarrisons(car, 2) = 0;
        elseif ((har_specific_carriers_above(car, 2) ~= 0) && (cha_specific_carriers_above(car,2) ~= 0))
            onlyHarrisons(car, 2) = 0;
        elseif ((har_specific_carriers_above(car, 2) ~= 0) && (cam_specific_carriers_above(car,2) ~= 0))
            onlyHarrisons(car, 2) = 0;
        elseif ((har_specific_carriers_above(car, 2) ~= 0) && (con_specific_carriers_above(car,2) ~= 0))
            onlyHarrisons(car ,2) = 0;
        elseif (har_specific_carriers_above(car, 2) ~= 0)
            ocount = ocount + 1;
        end
    end
    harrisonOnlyCarriers(counter, 2) = ocount;
    harrisonOnlyCarriers(counter, 1) = dblimit;
    counter = counter + 1;
end

save('harrisonOnlyCarriers.mat','harrisonOnlyCarriers');

function [specific_carriers_above, count] = specCarriers(file, scale, dblimit)
%%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

%%% INITIALIZES VALUES
[~,r,c] = size(file);

complete_signal = nan(64,r,c);    
complete_noisefloor = nan(64,r,c);
difference = nan(64,r,c);
carriers_above = nan(r,c);
best = [0,0,0];
mc = 0;

for temp = 1:r
    for loops = 1:c
            % Every other data point is noise, so the following start and
                % different numbers and get every other data point
        complete_signal(:,temp,loops) = file(1:2:end,temp,loops);    
        complete_noisefloor(:,temp,loops) = file(2:2:end,temp,loops);
        if isnan(complete_signal(:,temp,loops))
            difference(:,temp,loops) = nan;
        else
            difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
        end
        count = 0;
        for carriers = 1:64
            if ((difference(carriers,temp,loops) >= dblimit))
                count = count + 1;
            end
        end
        if isnan(difference(:,temp,loops))
            carriers_above(temp,loops) = nan;
        else
            carriers_above(temp,loops) = count;
        end
        if (count > mc)
            mc = count;
            best = [temp, loops, count];
        end
    end
end
%%%%%%%%%%%%%%%


%%%%% PART II - DETERMINES WHICH CARRIERS ARE ABOVE LIMIT @BEST POINT
specific_carriers_above = zeros(64,2);

temp = best(1,1);          %%% Assigns the indices of the best location
loops = best(1,2);
count = best(1,3);
for carriers = 1:64
    if ((temp <= 0) && (loops <= 0))
        specific_carriers_above(carriers,2) = 0;
    elseif difference(carriers,temp,loops) >= dblimit
        specific_carriers_above(carriers,2) = scale;
    end
end

%%%%%%%%%%%%%%%
clear complete_signal;
clear complete_noisefloor;
clear difference;
clear carriers_above;
end



