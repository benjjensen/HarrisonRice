useonearray = false;

if useonearray == false
    for runs = 1:NumSamples
        eval(sprintf('load("StationaryData/pwelch_%s_%d.mat");',room,runs));
        eval(sprintf('signal = pwelch_%s_%d(1:2:end,1);',room,runs));
        eval(sprintf('noise = pwelch_%s_%d(2:2:end,1);',room,runs));
        difference = abs(signal(:,1) - noise(:,1));
        count = 0;
        for carriers = 1:64
            if difference(carriers,1) >= db
                count = count + 1;
            end
        end
        eval(sprintf('%s_carriers_%d = count;',room,runs));
        eval(sprintf( ...
            'save("StationaryData/carriers_%s_%d.mat","carriers_%s_%d");' ...
            ,room,runs,room,runs));
    end
end
if useonearray == true
    eval(sprintf('load("StationaryData/pwelch_%s_array.mat");',room));
    for runs = 1:NumSamples
        eval(sprintf('signal = pwelch_%s_array(1:2:end,%d);',room,runs));
        eval(sprintf('noise = pwelch_%s_array(2:2:end,%d);',room,runs));
        difference = abs(signal(:,1) - noise(:,1));
        count = 0;
        for carriers = 1:64
            if difference(carriers,1) >= db
                count = count + 1;
            end
        end
        eval(sprintf('carriers_%s(:,%d) = count;',room,runs));
        clear signal;
        clear noise;
        clear difference;
    end
    eval(sprintf( ...
        'save("StationaryData/carriers_%s.mat","carriers_%s");' ...
        ,room,room));
end