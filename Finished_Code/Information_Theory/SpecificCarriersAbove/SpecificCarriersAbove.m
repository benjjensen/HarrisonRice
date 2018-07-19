
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

%%%%% SET DBLIMIT 
dblimit = 30;

for Harrison = 1:1   
    %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES 
    [~,r,c] = size(tx2harrison_pwelch); 
                        %(If changing this file, change lines 22 & 23 as well)

    complete_signal = nan(64,r-1,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r-1,c);
    difference = nan(64,r-1,c);
    carriers_above = nan(r-1,c);
    best = [0,0,0];
    mc = 0;
    
    for temp = 1:(r-1)
        for loops = 1:c
            complete_signal(:,temp,loops) = tx2harrison_pwelch(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = tx2harrison_pwelch(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,temp,loops) >= dblimit
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

    har_specific_carriers_above = zeros(64,2);

    temp = best(1,1);          %%% Assigns the indices of the best location
    loops = best(1,2);
    hcount = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           har_specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          har_specific_carriers_above(carriers,2) = 9;
      end
    end

    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end

for Smalley = 1:1                                                
    %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES 
    [~,r,c] = size(tx2smalley_pwelch); 
                        %(If changing this file, change lines 89 & 90 as well)

    complete_signal = nan(64,r-1,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r-1,c);
    difference = nan(64,r-1,c);
    carriers_above = nan(r-1,c);
    best = [0,0,0];
    mc = 0;
    
    for temp = 1:(r-1)
        for loops = 1:c
            complete_signal(:,temp,loops) = tx2smalley_pwelch(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = tx2smalley_pwelch(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,temp,loops) >= dblimit
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

    sma_specific_carriers_above = zeros(64,2);

    temp = best(1,1);          %%% Assigns the indices of the best location
    loops = best(1,2);
    scount = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           sma_specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          sma_specific_carriers_above(carriers,2) = 7;
      end
    end

    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end

for Chambers = 1:1                                                
    %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES
    [~,r,c] = size(tx2chambers_pwelch); 
                        %(If changing this file, change lines 157 & 158 as well)
                        
    complete_signal = nan(64,r-1,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r-1,c);
    difference = nan(64,r-1,c);
    carriers_above = nan(r-1,c);
    best = [0,0,0];
    mc = 0;


    for temp = 1:(r-1)
        for loops = 1:c
            complete_signal(:,temp,loops) = tx2chambers_pwelch(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = tx2chambers_pwelch(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,temp,loops) >= dblimit
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

    cha_specific_carriers_above = zeros(64,2);

    temp = best(1,1);          %%% Assigns the indices of the best location
    loops = best(1,2);
    chcount = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           cha_specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          cha_specific_carriers_above(carriers,2) = 5;
      end
    end
    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end

for Camacho = 1:1                                                
    %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES
    [~,r,c] = size(tx2camacho_pwelch); 
                        %(If changing this file, change lines 224 & 225 as well)
                        
    complete_signal = nan(64,r-1,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r-1,c);
    difference = nan(64,r-1,c);
    carriers_above = nan(r-1,c);
    best = [0,0,0];
    mc = 0;


    for temp = 1:(r-1)
        for loops = 1:c
            complete_signal(:,temp,loops) = tx2camacho_pwelch(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = tx2camacho_pwelch(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,temp,loops) >= dblimit
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

    cam_specific_carriers_above = zeros(64,2);

    temp = best(1,1);          %%% Assigns the indices of the best location
    loops = best(1,2);
    cacount = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           cam_specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          cam_specific_carriers_above(carriers,2) = 3;
      end
    end

    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end

for Conference = 1:1                                                
    %%%%% PART 1 - DETERMINES THE BEST LOCATION %%%%%

        %%% INITIALIZES VALUES
    [~,r,c] = size(tx2conference_pwelch); 
                        %(If changing this file, change lines 292 & 293 as well)
                        
    complete_signal = nan(64,r-1,c);     %%Uses the size of the data array, minus one
    complete_noisefloor = nan(64,r-1,c);
    difference = nan(64,r-1,c);
    carriers_above = nan(r-1,c);
    best = [0,0,0];
    mc = 0;


    for temp = 1:(r-1)
        for loops = 1:c
            complete_signal(:,temp,loops) = tx2conference_pwelch(1:2:end,temp,loops);
            complete_noisefloor(:,temp,loops) = tx2conference_pwelch(2:2:end,temp,loops);
            if isnan(complete_signal(:,temp,loops))
                difference(:,temp,loops) = nan;
            else
                difference(:,temp,loops) = abs(complete_signal(:,temp,loops) - complete_noisefloor(:,temp,loops));
            end
            count = 0;
            for carriers = 1:64
                if difference(carriers,temp,loops) >= dblimit
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

    con_specific_carriers_above = zeros(64,2);

    temp = best(1,1);          %%% Assigns the indices of the best location
    loops = best(1,2);
    cocount = best(1,3);
    for carriers = 1:64
      if ((temp <= 0) && (loops <= 0))
           con_specific_carriers_above(carriers,2) = 0;
      elseif difference(carriers,temp,loops) >= dblimit
          con_specific_carriers_above(carriers,2) = 1;
      end
    end

    %%%%%%%%%%%%%%%
    clear complete_signal;
    clear complete_noisefloor;
    clear difference;
    clear carriers_above;
end


%%%%% PART III - GRAPHS

hold on
title(['\fontsize{12} Tx_2 @' num2str(dblimit) ' dB']);
bar(har_specific_carriers_above(:,2),'DisplayName',['Harrison - ' num2str(hcount)]);
bar(sma_specific_carriers_above(:,2),'DisplayName',['Smalley - ' num2str(scount)]);
bar(cha_specific_carriers_above(:,2),'DisplayName',['Chambers - ' num2str(chcount)]);
bar(cam_specific_carriers_above(:,2),'DisplayName',['Camacho - ' num2str(cacount)]);
bar(con_specific_carriers_above(:,2),'DisplayName',['Conference - ' num2str(cocount)]);
legend;
hold off


