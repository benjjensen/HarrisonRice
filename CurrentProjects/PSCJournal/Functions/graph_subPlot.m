function graph_subPlot(roomName, type)
%graph_subPlot generates time plots for heavy, medium,and light data 

% This function loads the appropriate data and then plots three subplots of
% carrier vs magnitude, and then "plays" these plots like a movie.
%   ARGUMENTS:
%       roomName is a string containing the name of the desired office
%       type is either "linear" or "db" 
    
    %% Load data
    
    close all;

% Loads in appropriate data
eval(sprintf("load('../Data/LightMediumHeavy/%s/%s_heavy_%s');",type, roomName, type));
eval(sprintf("load('../Data/LightMediumHeavy/%s/%s_medium_%s');",type, roomName, type));
eval(sprintf("load('../Data/LightMediumHeavy/%s/%s_light_%s');",type, roomName, type));

eval(sprintf("heavyTraffic = %s_heavy_%s;",roomName,type));
eval(sprintf("mediumTraffic = %s_medium_%s;",roomName,type));
eval(sprintf("lightTraffic = %s_light_%s;",roomName,type));

    
    %% Plot Data
    
    [~, x] = size(heavyTraffic);

    for count = 1:x
        figure(1)
        subplot(3,1,1)
        sgtitle(roomName +": " + count + "th point");
        stem(heavyTraffic(:,count),'g');
        title("Heavy");
        if (type == "linear")
            axis([0 128 0 0.025]);
        else
            axis([0 128 -80 -10]);
        end
        xlabel('Carrier');
        ylabel("Magnitude (" + type + ")");
        
        subplot(3,1,2)
        stem(mediumTraffic(:,count),'b');
        title("Medium");
        if (type == "linear")
            axis([0 128 0 0.025]);
        else
            axis([0 128 -80 -10]);
        end
        xlabel('Carrier');
        ylabel("Magnitude (" + type + ")");
        
        subplot(3,1,3)
        stem(lightTraffic(:,count),'r');
        title("Light");
        if (type == "linear")
            axis([0 128 0 0.025]);
        else
            axis([0 128 -80 -10]);
        end
        xlabel('Carrier');
        ylabel("Magnitude (" + type + ")");
        
        set(gcf, 'Position',  [800, 100, 600, 850])
        drawnow;
    end 
end

