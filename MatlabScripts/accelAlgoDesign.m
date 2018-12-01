
function accelAlgoDesign(dist, lastDelay)

% code
distances = [];
delays = [];
t = 0;
times = [];
while dist ~= 0
    delay = calcAccelDelay(dist, lastDelay);
    times = [times, t];
    t = t + delay;
    delays = [delays, delay];
    distances = [distances, dist];
    lastDelay = delay;
    dist = dist - 1;
end

figure(1);
plot(distances, delays);
set(gca, 'XDir','reverse');
set(gca, 'YDir','reverse');

end


    
