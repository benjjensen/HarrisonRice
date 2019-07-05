%% Process Data (Pwelch + FFT)

dataArray = test;
startCarrier = 10;
endCarrier = 55;


[~, numSamples] = size(dataArray);
[processedData_linear, processedData_db] = processArray(dataArray, numSamples);

%% Separate noise and signal

signal_linear = processedData_linear(1:2:end, :);
noise_linear = processedData_linear(2:2:end, :);
signal_db = mag2db(signal_linear);
noise_db = mag2db(noise_linear);

%% Normalize
        % Needs to all be normalized to one thing - this isnt done

normalized_signal_linear = normalize_signal(signal_linear,noise_linear);
normalized_signal_db = mag2db(normalized_signal_linear);

%% Plot

graph_plot(normalized_signal_linear, normalized_signal_db, noise_db);


%% Save ?

function graph_plot(signal_linear, signal_db, noise_db)
%graph_plot generates time plots for heavy, medium,and light data 

% This function 
    

    %% Plot Data
    close all;
    
    [~, x] = size(signal_linear);

    for count = 1:x
        figure(1)
        subplot(3,1,1)
        sgtitle(count + "th point");
        stem(signal_linear(:,count),'g');
        title("Linear - Signal");
        axis([0 64 0 0.05]);
        xlabel('Carrier');
        ylabel("Magnitude");
        
        subplot(3,1,2)
        stem(signal_db(:,count),'b');
        title("dB - signal");
        axis([0 64 -80 -10]);
        xlabel('Carrier');
        ylabel("Magnitude (dB)");    
        
        subplot(3,1,3)
        stem(noise_db(:, count),'r');
        title("dB - noise");
        axis([0 64 -120 -20]);
        
        set(gcf, 'Position',  [800, 100, 600, 850])
        drawnow;
    end 
end

