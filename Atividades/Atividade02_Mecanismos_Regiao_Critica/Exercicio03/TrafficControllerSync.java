public class TrafficControllerSync {

    private boolean isBridgeFree = false;

    public void SetBridgeState(boolean value) {
        this.isBridgeFree = value;
    }

    public boolean GetBridgeState() {
        return this.isBridgeFree;
    }

    public synchronized void crossBridge() {
        while (this.GetBridgeState() == true) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.out.println(e);
                break;
            }
        }
        isBridgeFree = true;
        notifyAll();
    }

    public synchronized void exitBridge() {
        this.SetBridgeState(false);
        notifyAll();
    }

    public void enterLeft() {
        crossBridge();
    }

    public void enterRight() {
        crossBridge();
    }

    public void leaveLeft() {
        exitBridge();
    }

    public void leaveRight() {
        exitBridge();
    }

}