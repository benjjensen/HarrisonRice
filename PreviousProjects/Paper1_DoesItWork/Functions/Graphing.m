function [] = Graphing(dataArray,isRawData,closeOtherFigures)
% Created by Kalin Norman
% This file takes a data array (either raw data or the processed data in
% dB) and plots up to 20 frames of the data (the frames that are desired
% should be selected and passed in, rather than the full data array, should
% you desire to plot specific locations, otherwise only the first 20
% frames in the data will be plotted)
% Parameters:
% dataArray - raw data or processed data in dB
% isRawData - true or false, indicating what type of data you passed in
% closeOtherFigures - true or false, whether or not close all should be run

if (closeOtherFigures)
    close all;
end
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
% Checks the size of the data array to only allow up to 20 plots (an
% excessive number of plots can crash MATLAB or even your computer, a
% fairly reasonable number has been chosen to work on lower-end machines as
% well)
[X,Y,Z] = size(dataArray);
if (Y * Z > 20)
    numPlots = 20;
    if (Z > 5)
        numRows = 4;
        numCols = 5;
    else
        numCols = Z;
        numRows = 20 / Z;
    end
else
    numPlots = Y * Z;
    numRows = Y;
    numCols = Z;
end

% Code to generate plots if the raw data from the Pluto radio is passed in
if (isRawData)
    for runs = 1:numPlots
        YY = pwelch(dataArray(:,runs),boxcar(Nfft),0,Nfft,'twosided');
        YYplot = 10*log10(abs(fftshift(YY)));
        figure(runs);
        stem(FF,YYplot);
        grid on;
        xlabel('frequency (MHz)');
        ylabel('Magnitude (dB)');
        ax = gca;
        ax.Children.BaseValue = -80;
        hold on;
        plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
        hold off;
    end
    
% Generates the plots if data in dB is passed in
% If linear data is pased in this will also work, but the line
% "ac.Children.BaseValue = -80" will mess up the view and make it difficult
% to read.
else
    for Row = 1:numRows
        for Col = 1:numCols
            YYplot = dataArray(:,Row,Col);
            figure();
            stem(FF,YYplot);
            title('(' + string(Row) + ',' + string(Col) + ')');
            grid on;
            xlabel('frequency (MHz)');
            ylabel('Magnitude (dB)');
            ax = gca;
            ax.Children.BaseValue = -80;
            hold on;
            plot(FF(1:2:end),YYplot(1:2:end),'o','MarkerFaceColor','k');
            hold off;
        end
    end
end
end

