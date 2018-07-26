

close all;
load('chambers_pwelch.mat');
load('complete_pwelch.mat');
load('conference_pwelch.mat');
load('harrison_pwelch.mat');
load('smalley_pwelch.mat');

load('tx2_pwelch.mat');
load('tx2camacho_pwelch.mat');
load('tx2chambers_pwelch.mat');
load('tx2conference_pwelch.mat');
load('tx2harrison_pwelch.mat');
load('tx2smalley_pwelch.mat');
    
    % Number of Carriers above the dblimit (tx2)
stepSize = .1;
maxDB = 35;
har_carriersPerDB = carriersPerDB(tx2harrison_pwelch, stepSize, maxDB);
sma_carriersPerDB = carriersPerDB(tx2smalley_pwelch, stepSize, maxDB);
cam_carriersPerDB = carriersPerDB(tx2camacho_pwelch, stepSize, maxDB);
cha_carriersPerDB = carriersPerDB(tx2chambers_pwelch, stepSize, maxDB);
con_carriersPerDB = carriersPerDB(tx2conference_pwelch, stepSize, maxDB);


hold on
title({['\fontsize{12}Carriers Per DB'] ; ...
                ['\fontsize{11}Step Size = ' num2str(stepSize)]});
plot(har_carriersPerDB,'DisplayName','Harrison');
plot(sma_carriersPerDB,'DisplayName','Smalley');
plot(cam_carriersPerDB,'DisplayName','Camacho');
plot(cha_carriersPerDB,'DisplayName','Chambers');
plot(con_carriersPerDB,'DisplayName','Conference');
legend;
grid minor
hold off


function matrix = carriersPerDB(file, stepSize, maxDB)

    [~, r, c] = size(file);
    matrix = zeros(1,(maxDB/stepSize)+9);       %The +9 just helps the i's come out nicer
    i = 9;   

    for dblimit = 1:stepSize:maxDB
        
        [bestRow, bestCol] = specCarriers(file, 1, dblimit);       
        i = i + 1;
        if (bestRow == 0 || bestCol == 0)
            matrix(i) = 0;
        else
            complete_signal = nan(64,r,c);       
            complete_noisefloor = nan(64,r,c);
            difference = nan(64,r,c);       

            complete_signal(:,bestRow,bestCol) = file(1:2:end,bestRow,bestCol);
            complete_noisefloor(:,bestRow,bestCol) = file(2:2:end,bestRow,bestCol);
            if isnan(complete_signal(:,bestRow,bestCol))
                difference(:,bestRow,bestCol) = nan;
            else
                difference(:,bestRow,bestCol) = abs(complete_signal(:,bestRow,bestCol) - complete_noisefloor(:,bestRow,bestCol));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,bestRow,bestCol) >= dblimit
                    count = count + 1;
                end
            end    
            matrix(i) = count;
        end
    end
end

function [temp, loops] = specCarriers(file, scale, dblimit)
 %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES 
    [~,r,c] = size(file); 
                        %(If changing this file, change lines 22 & 23 as well)

    complete_signal = nan(64,r,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r,c);
    difference = nan(64,r,c);
    carriers_above = nan(r,c);
    best = [0,0,0];
    mc = 0;
    
    for temp = 1:(r)
        for loops = 1:c
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
