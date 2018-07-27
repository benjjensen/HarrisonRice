% Determines which specific carriers are above a set dblimit for each room
    % Graphs them on a graph, scaled so you can see each room. Uses Tx2
    
close all;  
load('camacho_pwelch.mat');
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

%%% SET DBLIMIT 
dblimit = 28;

            %Different scales are used to allow us to see each on the graph 
    [har_specific_carriers_above, hcount] = specCarriers(tx2harrison_pwelch, 9, dblimit);
    [sma_specific_carriers_above, scount] = specCarriers(tx2smalley_pwelch, 7, dblimit);
    [cha_specific_carriers_above, chcount] = specCarriers(tx2chambers_pwelch, 5, dblimit);
    [cam_specific_carriers_above, cacount] = specCarriers(tx2camacho_pwelch, 3, dblimit);
    [con_specific_carriers_above, cocount] = specCarriers(tx2conference_pwelch, 1, dblimit);

            %%%%% OPTIONAL - COMPARE ARRAYS TO HARRISONS
    for car = 1:64
        if ((har_specific_carriers_above(car, 2) == 0) && (sma_specific_carriers_above(car,2) ~= 0))
            sma_specific_carriers_above(car,2) = 0;
            scount = scount - 1;
        end
        if ((har_specific_carriers_above(car, 2) == 0) && (cha_specific_carriers_above(car,2) ~= 0))
            cha_specific_carriers_above(car,2) = 0;
            chcount = chcount - 1;
        end
        if ((har_specific_carriers_above(car, 2) == 0) && (cam_specific_carriers_above(car,2) ~= 0))
            cam_specific_carriers_above(car,2) = 0;
            cacount = cacount - 1;
        end
        if ((har_specific_carriers_above(car, 2) == 0) && (con_specific_carriers_above(car,2) ~= 0))
            con_specific_carriers_above(car,2) = 0;
            cocount = cocount - 1;
        end
    end
          

%%%%% PART III - GRAPHS
figure()
hold on
title(['\fontsize{12} Tx_2 @' num2str(dblimit) ' dB']);
bar(har_specific_carriers_above(:,2),'DisplayName',['Harrison - ' num2str(hcount)]);
bar(sma_specific_carriers_above(:,2),'DisplayName',['Smalley - ' num2str(scount)]);
bar(cha_specific_carriers_above(:,2),'DisplayName',['Chambers - ' num2str(chcount)]);
bar(cam_specific_carriers_above(:,2),'DisplayName',['Camacho - ' num2str(cacount)]);
bar(con_specific_carriers_above(:,2),'DisplayName',['Conference - ' num2str(cocount)]);
legend;
xlabel('dB');
set(gca,'YTickLabel',[]);
hold off

for ClearVariables = 1:1
    clear c;
    clear cacount;
    clear Camacho;
    clear carriers;
    clear Chambers;
    clear chcount;
    clear cocount;
    clear Conference;
    clear count;
    clear Harrison;
    clear hcount
    clear loops;
    clear mc;
    clear r;
    clear scount;
    clear Smalley;
    clear temp;
    clear best;
end
    clear ClearVariables;
   
    
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
    count = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          specific_carriers_above(carriers,2) = scale;
      end
    end

    %%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end



