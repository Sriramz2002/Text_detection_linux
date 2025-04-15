import cv2
import easyocr
from langdetect import detect
import time
import re
from collections import Counter

reader = easyocr.Reader(['en', 'hi'], verbose=False)

def is_probably_english(text):
    return all(ord(c) < 128 for c in text) and len(text.split()) <= 3

# Auto-detect working camera
for i in range(5):
    cap = cv2.VideoCapture(i)
    if cap.isOpened():
        print(f"[INFO] Using camera index: {i}")
        break
    cap.release()
else:
    print("[ERROR] No working camera found.")
    exit()

print("[INFO] Starting webcam OCR. Press 'q' to quit.")

frame_count = 0
text_counter = Counter()
finalized_set = set()
detected_texts = []

while True:
    ret, frame = cap.read()
    if not ret:
        break

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = reader.readtext(rgb)

    combined_text = ""
    for bbox, text, conf in results:
        if conf > 0.5 and len(text.strip()) >= 3:
            clean_text = re.sub(r'[^\w\s]', '', text).strip()
            combined_text += clean_text + " "

            # Draw bounding box
            top_left = tuple(map(int, bbox[0]))
            bottom_right = tuple(map(int, bbox[2]))
            cv2.rectangle(frame, top_left, bottom_right, (0, 255, 0), 2)

            # Label with text + confidence
            label = f"{clean_text} ({conf:.2f})"
            cv2.putText(frame, label, (top_left[0], top_left[1] - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

    combined_text = combined_text.strip()

    if combined_text:
        detected_texts.append(combined_text)
        text_counter[combined_text] += 1

        if text_counter[combined_text] == 2 and combined_text not in finalized_set:
            if is_probably_english(combined_text):
                lang = 'en'
            else:
                try:
                    lang = detect(combined_text)
                except:
                    lang = "unknown"

            print(f"[LIVE FINALIZED] Language: {lang.upper()} | Text: {combined_text}")
            finalized_set.add(combined_text)

    cv2.imshow("Webcam OCR (EasyOCR)", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

# ===========================
# FINALIZATION PHASE
# ===========================
if detected_texts:
    common_texts = [text for text, count in text_counter.items() if count > 3]

    if common_texts:
        with open("output.txt", "w", encoding="utf-8") as f:
            for text in common_texts:
                try:
                    lang = detect(text)
                except:
                    lang = "unknown"
                f.write(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}]\n")
                f.write(f"Language: {lang.upper()}\nText: {text}\n\n")
    else:
        print("\n[INFO] No repeated text detected.")
else:
    print("\n[INFO] No text detected.")
