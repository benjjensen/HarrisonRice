%% Created by Bradford Clark
% March 6, 2019
% This file creates figures that show at what position Eave gets a given
% revealed number of bits and where bob can recieve all of the given bits

%% Actual File
close all
addpath('../Functions');
addpath('../Data');
load('V2InumCarriersPerLocation.mat');
load('SNRForV2I.mat');

syms x;

for k = 15:15
    maxNumberOfCarriers = 32;
    carriersPerLocation = cap_alpha(1:10:end,k);
    numberOfSpots = length(carriersPerLocation);
    
    scaleEave = double(solve(435.2 == x * numberOfSpots,x));
    u = 2;
    m = 5;
    
    kBits = 0;
    for i = 0 : u
        kBits = kBits + nchoosek(m,i);
    end
    
    RMWeights = RMWeightHier(u,m,false);
    
    bobsKnowledge = zeros(numberOfSpots);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    for i = 1:numberOfSpots
        if carriersPerLocation(i) == 32
            bobsKnowledge(:,2*i-1:2*i) = 0;
        else
            bobsKnowledge(:,2*i-1:2*i) = NaN;
        end
    end
    figure(100+k);
    hold on;
    % bob = surf(flipud(bobsKnowledge));%-maxNumberOfCarriers);
    % grid off;
    % % cData = [1 1 1];
    % caxis([0 32]);
    % view(0,90);
    % myColorMap = ones(128,3); % Set row 128 to white.
    % % Then apply the colormap
    % colormap(myColorMap);
    
    
    
    %%
    %%%%%% Find Eves matrix %%%%%%
    eavesKnowledge = zeros(numberOfSpots);
    
    for i = 1:numberOfSpots
        eavesKnowledge(i,:) = RMWeights(33-(ceil(carriersPerLocation(i))));
    end
    
    [eavey eavex] = size(eavesKnowledge);
    xaxis = zeros(eavex);
    yaxis = zeros(eavey);
    for i = 1:32
        xaxis(:,i) = i;
        yaxis(i,:) = i;
    end
    myColorMapScatter = jet(kBits+1);
    colormap(myColorMapScatter);
    
    
    % for i = 1:eavex
    %
    %     eave = scatter3(xaxis(i,:),yaxis(i,:),flipud(eavesKnowledge(i,:)),'s',...
    %         'MarkerFaceColor',myColorMapScatter(eavesKnowledge(i,1)+1,:),...
    %         'MarkerEdgeColor',myColorMapScatter(eavesKnowledge(i,1)+1,:));
    %
    %
    %
    %     view(0,90);
    % %     set(h, 'SizeData', markerWidth^2)
    %     eave.SizeData = 100;
    %     eave.MarkerFaceAlpha = .5;
    %     eave.MarkerEdgeAlpha = .5;
    %     drawnow;
    % end
    % colormap(myColorMapScatter);
    % colorbar;
    % caxis([-3 n]);
    
    % eave = contourf(flipud(eavesKnowledge));
    % patches = findobj(eave,'-property','AlphaData');
    % for ph = patches
    %  set(ph,'AlphaData', 1 * get(ph,'AlphaData'));
    % end
    xyaxis = 0:scaleEave:eavex * scaleEave - scaleEave;
    
    eave = pcolor(xyaxis,xyaxis,eavesKnowledge);
    set(eave,'EdgeAlpha',0);
    colorBar = colorbar;
    colorBar.Label.String = 'bits revealed to Eave';
    colorBar.Label.FontSize = 15;
    caxis([0 kBits]);
    xlabel('Bobs Position');
    ylabel('Eaves Position');
    title('RM(1,5)');
    % clabel('bits revealed to Eave');
    hold off;
    
    % xlim([0 32]);
    % ylim([0 32]);
    cf = gcf;
    cf.PaperSize = [5 4];
    cf.PaperPosition = [-.05521 0.2240 -0.5521+5 0.2240+4];
    
    % %%
    % figure(100000)
    % imagesc(flipud(eavesKnowledge), 'AlphaData', .4)
    
    %%search for cases where bob gets all 32 bits
    placesBobGetsAll = strfind(carriersPerLocation',maxNumberOfCarriers)';
    countIndex = 1;
    placesBobGetsAllArea = [];
    for i = 1:length(placesBobGetsAll)
        if ((i == 1) &&...
                (placesBobGetsAll(i) + 1 == placesBobGetsAll(i+1)))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        elseif ((i == length(placesBobGetsAll)) &&...
                (placesBobGetsAll(i) - 1 == placesBobGetsAll(i-1)))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        elseif (i ~= 1 && i ~= length(placesBobGetsAll)) && xor((placesBobGetsAll(i-1) == placesBobGetsAll(i) - 1),(placesBobGetsAll(i+1) == placesBobGetsAll(i) + 1))
            placesBobGetsAllArea(countIndex) = placesBobGetsAll(i);
            countIndex = countIndex + 1;
        end
        
    end
    
    
    for i = 1:2:length(placesBobGetsAllArea)
        firstSpot = placesBobGetsAllArea(i) * scaleEave;
        secondSpot = placesBobGetsAllArea(i+1) * scaleEave;
        bobPatch1 = patch([firstSpot firstSpot secondSpot secondSpot],...
            [scaleEave eavex*scaleEave eavex*scaleEave scaleEave],'white');
        
        % Get patch objects from CONTOURGROUP
        bobHandle = findobj(bobPatch1, 'Type', 'patch');
        
        % Apply Hatch Fill
        bobHatch1 = hatchfill(bobHandle, 'cross', 45, 4);
        
        % Remove outline
        set(bobPatch1, 'LineStyle', 'none');
        
        % Change the cross hatch to white
        set(bobHatch1, 'Color', 'white');
        
    end
    
    % xlim([1 32]);
    % ylim([1 32]);
    hold off;
    drawnow;
%     toc;
end

