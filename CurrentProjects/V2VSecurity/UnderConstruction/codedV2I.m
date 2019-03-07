%Created by Bradford Clark
% March 6, 2019
% close all
addpath('../Functions');
addpath('../Data');
load('BradfordShouldPayAttention.mat');
% number = 32;
% for i = 1:number
%     if i == 31 || i == 30
%         fakeData(1,i) = mod(32,33);
%     else
%         fakeData(1,i) = mod(i,33);
%     end
% end
maxNumberOfCarriers = 32;
secrecyCapacityPerLocation = floor(capacity(1:10:length(capacity),1)'...
    * maxNumberOfCarriers);
% secrecyCapacityPerLocation = fakeData;
numberOfSpots = length(secrecyCapacityPerLocation);
r = 2;
m = 5;
n = 16;
RMWeights = RMWeightHier(r,m,false);

bobsKnowledge = zeros(numberOfSpots);

for i = 1:numberOfSpots
    if secrecyCapacityPerLocation(i) == 32
        bobsKnowledge(:,i) = 0;
    else
        bobsKnowledge(:,i) = NaN;
    end
end
figure(101);
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
    eavesKnowledge(i,:) = RMWeights(33-(ceil(secrecyCapacityPerLocation(i)+1)));
end

[eavey eavex] = size(eavesKnowledge);
xaxis = zeros(eavex);
yaxis = zeros(eavey);
for i = 1:32
    xaxis(:,i) = i;
    yaxis(i,:) = i;
end
myColorMapScatter = jet(n+1);
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

eave = pcolor(eavesKnowledge);
set(eave,'EdgeAlpha',0);
colorBar = colorbar;
colorBar.Label.String = 'bits revealed to Eave';
colorBar.Label.FontSize = 15;
xlabel('Bobs Position');
ylabel('Eaves Position');
title('RM(2,5)');
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

% close all
bobPatch1 = patch([232 232 231 231],[1 eavex eavex 1],'white');

% Get patch objects from CONTOURGROUP
bobHandle = findobj(bobPatch1, 'Type', 'patch');

% Apply Hatch Fill
bobHatch1 = hatchfill(bobHandle, 'cross', 45, 4);

% Remove outline
set(bobPatch1, 'LineStyle', 'none');

% Change the cross hatch to white
set(bobHatch1, 'Color', 'white');

% xlim([1 32]);
% ylim([1 32]);
hold off;

