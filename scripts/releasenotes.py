"""
Create a first draft of release notes for a new version of PROJ

The script outputs a section for the NEWS.md file. It's based on the title of
entries added to the specified GitHub milestone. The script tries to group the
entries into "updates" and "bug fixes" although it might not always be
succesful.

The output of the scripts should be edited before adding to NEWS.md. Less editing
is needed if PR's are given good titles when originally created.

The script requires a token from GitHub. These can be generated under the "Settings"
section in your GitHub profile. Go to "Developer Settings" and then "Personal access
tokens". The token does not need any special permissions.

Usage:

    python releasenotes.py <milestone name> <github api token>
"""

import argparse

import requests

# Base API URL to fetch issues associated with a milestone
REPO = "OSGeo/PROJ"
STATE = "closed"
BASE_URL = "https://api.github.com/repos/OSGeo/PROJ/issues"


def get_milestone_number(repo, milestone_title, headers):
    """Fetch the milestone number by title."""
    url = f"https://api.github.com/repos/{repo}/milestones"
    response = requests.get(url, headers=headers)
    milestones = response.json()
    for milestone in milestones:
        if milestone["title"] == milestone_title:
            return milestone["number"]
    return None


def fetch_issues(url, params, headers):
    """Fetch all issues for a milestone with pagination."""
    issues = []
    while url:
        response = requests.get(url, headers=headers, params=params)
        response_data = response.json()
        issues.extend(response_data)
        # Check if there is a next page
        if "next" in response.links:
            url = response.links["next"]["url"]
        else:
            url = None
    return issues


def is_merged(pr_url, headers):
    """Check if a pull request is merged."""
    pr_response = requests.get(pr_url, headers=headers)
    pr_data = pr_response.json()
    return pr_data.get("merged_at") is not None


def main(milestone_title, token):
    headers = {
        "Authorization": f"token {token}"
    }

    milestone_number = get_milestone_number(REPO, milestone_title, headers)

    if milestone_number is None:
        print(f"Milestone '{milestone_title}' not found.")
        return

    params = {
        "milestone": milestone_number,
        "state": STATE,
        "per_page": 100  # Fetch 100 results per page
    }

    issues = fetch_issues(BASE_URL, params, headers)

    pull_requests = [issue for issue in issues if "pull_request" in issue]

    new_features = []
    bug_fixes = []

    for pr in pull_requests:
        pr_url = pr["pull_request"]["url"]
        if is_merged(pr_url, headers):
            title = pr["title"]
            if "fix" in title.lower() or "bug" in title.lower():
                bug_fixes.append((pr["number"], title))
            else:
                new_features.append((pr["number"], title))

    print(f"## {milestone_title}\n")

    print("### Updates\n")
    for pr in reversed(new_features):
        print(f"* {pr[1]} (#{pr[0]})\n")

    print("\n### Bug Fixes\n")
    for pr in reversed(bug_fixes):
        print(f"* {pr[1]} (#{pr[0]})\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate release notes for a specific milestone.")
    parser.add_argument("milestone", type=str, help="The title of the milestone")
    parser.add_argument("token", type=str, help="Your personal access token")
    args = parser.parse_args()

    main(args.milestone, args.token)
