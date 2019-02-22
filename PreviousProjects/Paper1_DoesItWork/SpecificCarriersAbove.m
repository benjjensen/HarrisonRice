% Determines which specific carriers are above a set dblimit for each room
    % Graphs them on a graph, scaled so you can see each room. Uses Tx2
    
    % (NOTE - functions need to be added to the path before this is used)
    
%% Loads in the data from paper 1 (snapshot of Clyde building fourth floor)    
    
close all;  

close all;  
load('Data/tx2dB.mat');
load('Data/tx1dB.mat');

    % Assigns the correct rows and columns to the appropriate room 
tx2_pwelch = tx2dB;
tx2conference_pwelch = tx2dB(:, 1:65, 1:61);
tx2harrison_pwelch = tx2dB(:, 36:65, 65:98);
tx2smalley_pwelch = tx2dB(:, 34:65, 123:148);
tx2camacho_pwelch = tx2dB(:, 34:65, 154:179);
tx2chambers_pwelch = tx2dB(:, 34:65, 213:240);

tx1_pwelch = tx1dB;
tx1conference_pwelch = tx1dB(:, 1:65, 1:61);
tx1harrison_pwelch = tx1dB(:, 37:65, 65:97);
tx1smalley_pwelch = tx1dB(:, 34:65, 123:148);
tx1camacho_pwelch = tx1dB(:, 34:65, 154:179);
tx1chambers_pwelch = tx1dB(:, 34:65, 213:240);


%% SET DBLIMIT 
dblimit = 25.8;

            % Arbitrary scales are used to allow us to see each on the graph 
    [har_specific_carriers_above, hcount] = findSpecCarrier(tx2harrison_pwelch, 9, dblimit);
    [sma_specific_carriers_above, scount] = findSpecCarrier(tx2smalley_pwelch, 7, dblimit);
    [cha_specific_carriers_above, chcount] = findSpecCarrier(tx2chambers_pwelch, 5, dblimit);
    [cam_specific_carriers_above, cacount] = findSpecCarrier(tx2camacho_pwelch, 3, dblimit);
    [con_specific_carriers_above, cocount] = findSpecCarrier(tx2conference_pwelch, 1, dblimit);

            %% OPTIONAL - COMPARE ARRAYS TO HARRISONS
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
          

%% PART III - GRAPHS
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
