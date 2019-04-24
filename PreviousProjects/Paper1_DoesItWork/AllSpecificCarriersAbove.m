% Determines which specific carriers are above a set dblimit for each room
    % Graphs them on a graph, scaled so you can see each room. Uses Tx2
    
    % needs to be updated, data is processed in the old way
    
close all;  
load('Data/tx2dB.mat');

    % Assigns the correct rows and columns to the appropriate room 
tx2_pwelch = tx2dB;
tx2conference_pwelch = tx2dB(:, 1:65, 1:61);
tx2harrison_pwelch = tx2dB(:, 36:65, 65:98);
tx2smalley_pwelch = tx2dB(:, 34:65, 123:148);
tx2camacho_pwelch = tx2dB(:, 34:65, 154:179);
tx2chambers_pwelch = tx2dB(:, 34:65, 213:240);

%%% SET PARAMETERS
dblimit = 25.8;
minCarrier = 1;
maxCarrier = 64;

            %Different scales are used to allow us to see each on the graph 
    [har_specific_carriers_above, hcount] = specCarriers(tx2harrison_pwelch, 9, dblimit, minCarrier, maxCarrier);
    [sma_specific_carriers_above, scount] = specCarriers(tx2smalley_pwelch, 7, dblimit, minCarrier, maxCarrier);
    [cha_specific_carriers_above, chcount] = specCarriers(tx2chambers_pwelch, 5, dblimit, minCarrier, maxCarrier);
    [cam_specific_carriers_above, cacount] = specCarriers(tx2camacho_pwelch, 3, dblimit, minCarrier, maxCarrier);
    [con_specific_carriers_above, cocount] = specCarriers(tx2conference_pwelch, 1, dblimit, minCarrier, maxCarrier);

%     save('har_specific_carriers_above.mat','har_specific_carriers_above');
            %%%% OPTIONAL - COMPARE ARRAYS TO HARRISONS
%     for car = minCarrier:maxCarrier
%         if ((har_specific_carriers_above(car, 2) == 0) && (sma_specific_carriers_above(car,2) ~= 0))
%             sma_specific_carriers_above(car,2) = 0;
%             scount = scount - 1;
%         end
%         if ((har_specific_carriers_above(car, 2) == 0) && (cha_specific_carriers_above(car,2) ~= 0))
%             cha_specific_carriers_above(car,2) = 0;
%             chcount = chcount - 1;
%         end
%         if ((har_specific_carriers_above(car, 2) == 0) && (cam_specific_carriers_above(car,2) ~= 0))
%             cam_specific_carriers_above(car,2) = 0;
%             cacount = cacount - 1;
%         end
%         if ((har_specific_carriers_above(car, 2) == 0) && (con_specific_carriers_above(car,2) ~= 0))
%             con_specific_carriers_above(car,2) = 0;
%             cocount = cocount - 1;
%         end
%     end
          

%%%%% PART III - GRAPHS
figure()
hold on
title(['\fontsize{12} Tx_2 @' num2str(dblimit) ' dB']);
bar(har_specific_carriers_above(:,2),'DisplayName',['Harrison - ' num2str(hcount)]);
bar(sma_specific_carriers_above(:,2),'DisplayName',['Smalley - ' num2str(scount)]);
bar(cha_specific_carriers_above(:,2),'DisplayName',['Chambers - ' num2str(chcount)]);
bar(cam_specific_carriers_above(:,2),'DisplayName',['Camacho - ' num2str(cacount)]);
bar(con_specific_carriers_above(:,2),'DisplayName',['Conference - ' num2str(cocount)]);
xlabel('Carrier Number');
xlim([minCarrier maxCarrier]);
set(gca,'YTickLabel',[]);
legend;      
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
   
    
function [specific_carriers_above, count] = specCarriers(file, scale, dblimit, minCarrier, maxCarrier)
 %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES 
    [~,r,c] = size(file);                         

    complete_signal = nan(64,r,c);     
    complete_noisefloor = nan(64,r,c);
    difference = nan(64,r,c);     
    specific_carriers_above = zeros(64,2); 
    
    for temp = 1:(r)
        for loops = 1:c
            complete_signal(:,temp,loops) = file(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = file(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end            
            for carriers = minCarrier:maxCarrier
                if ((difference(carriers,temp,loops) >= dblimit))                                   
                    specific_carriers_above(carriers,2) = scale;
                end
            end             
        end
    end  
    count = 0;  % Reset count each time
    for carriers = minCarrier:maxCarrier
        if (specific_carriers_above(carriers,2) ~= 0) % Count the non-zeros
            count = count + 1;
        end
    end

    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;   
end