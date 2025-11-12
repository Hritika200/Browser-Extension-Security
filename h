import os
import requests
from urllib.parse import quote

# Folder with your CRX filenames or IDs
crx_dir = "crx_files"
output_dir = "downloaded_zips"

os.makedirs(output_dir, exist_ok=True)

# Chrome version to spoof — newer versions often yield valid CRX3 payloads
CHROME_VERSION = "120.0.0.0"

def download_extension_zip(extension_id):
    """Fetch CRX and save the actual extension ZIP."""
    print(f"Fetching {extension_id} ...")
    update_url = (
        f"https://clients2.google.com/service/update2/crx?"
        f"response=redirect&prodversion={CHROME_VERSION}&acceptformat=crx2,crx3&"
        f"x=id%3D{quote(extension_id)}%26installsource%3Dondemand%26uc"
    )

    # Save as .zip instead of .crx
    out_path = os.path.join(output_dir, f"{extension_id}.zip")
    headers = {
        "User-Agent": f"Chrome/{CHROME_VERSION}",
    }

    try:
        r = requests.get(update_url, headers=headers, allow_redirects=True, timeout=30)
        r.raise_for_status()
        # If the returned content starts with 'Cr24', it's a CRX header; trim if needed.
        data = r.content
        if data[:4] == b"Cr24":
            header_end = data.find(b"PK\x03\x04")
            if header_end != -1:
                data = data[header_end:]
        with open(out_path, "wb") as f:
            f.write(data)

        print(f"Saved ZIP: {out_path}")
    except Exception as e:
        print(f"Error downloading {extension_id}: {e}")

# Loop through all CRX files and infer IDs
for file in os.listdir(crx_dir):
    if file.endswith(".crx") or len(file) == 32:  # Chrome IDs are 32 chars
        ext_id = os.path.splitext(file)[0]
        download_extension_zip(ext_id)

print("\nAll extensions downloaded as ZIPs in:", output_dir)
