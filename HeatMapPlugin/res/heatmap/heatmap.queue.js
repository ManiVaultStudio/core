/**
 * Adapted from ReScatter SelectionSubscriber  28-03-2018: Baldur van Lew
 */
/**
 * @constructor
//<Class definition for the HeatmapDataQueue>
 * @param {queueLimit} - required integer to limit the number of
 * dynamic selection events held in the queue. If set a maximum of
 * the latest dynamicLimit events are saved.
 * @param (processingCallback) - required callback to process the data
 * should accept the data parameter that was added to the queue with addData.
 *
 * Create a queue with the callback and fee it data.
 * it will only handle the most recent queueLimit items the rest are dropped
 */

(function (global) {

function HeatmapDataQueue(queueLimit, processingCallback) {
    this.dataQueue = new CBuffer(queueLimit);
    this.processingCallback = processingCallback;
    this.processingDataQueue = false;
    this.ready = false;
    this.processLoopTimer = undefined;
    this.processWatchdogTimer = undefined;
    this.processTimedout = true;
}

HeatmapDataQueue.prototype = {

    constructor: HeatmapDataQueue,

    addData: function(data) {
        "use strict";
        this.dataQueue.push(data);
        if (this.processTimedout) {
            this.processingLoop(true);
        }
    },

    processingDone: function() {
        "use strict";
        if (this.processWatchdogTimer) {
            clearTimeout(this.processWatchdogTimer);
        }
        this.processTimedout = false;
        this.ready = true;
    },

    /**
     * @param force - force the processing loop to process the next event
     *
     * Call with force=true to start the loop from scratch.
     * The processing loop is called via a timeout every 10ms.
     * It checks if data processing is finished (indicated by this.ready)
     * and if so triggers processing of the next event.
     */
    processingLoop: function(force) {
        "use strict";
        var self = this;
        if (force && (this.processLoopTimer !== undefined)) {
            clearTimeout(this.processLoopTimer);
            if (this.processWatchdogTimer) {
                clearTimeout(this.processWatchdogTimer);
            }
            this.processTimedout = false;
        }
        this.ready = force || this.ready;
        if (this.ready) {
            this.__processNextEvent();
        }

        this.processLoopTimer  = setTimeout(function () {
            self.processingLoop(false);
        }, 10);
    },


    /**
     * @private
     * Get the next data to be processed, set this.ready to false to indicate
     * processing in progress and start processing.
     * A watchdog time with a timeout of 500ms is started to trap any failing
     * processing 
     */
    __processNextEvent: function() {
        "use strict";
        var self = this;
        var data = this.dataQueue.shift();
        if (data) {
            //console.log("Event id: ", event.id);
            this.ready = false;
            this.processTimedout = false;
            if (this.processWatchdogTimer) {
                clearTimeout(this.processWatchdogTimer);
            }
            //console.log("Processing event " + this.dumpEvent(event, 2) + " in " + this.id);
            this.processWatchdogTimer = setTimeout(function() {self.processTimedout = true;}, 500);
            this.processData(data);
        }
    },

    /**
     * @param data - the heatmap data 
     */
    processData: function(data) {
        "use strict";
        try{
            this.processingCallback(data);
        }
        catch(e) {
            console.log('Error processing data: ' + e)
        }
        finally {
            this.processingDone();
        }
    }

};

if (typeof module === 'object' && module.exports) module.exports = HeatmapDataQueue;
else global.HeatmapDataQueue = HeatmapDataQueue;

}(this));

